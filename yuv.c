/*
 * SDLCL - SDL Compatibility Library
 * Copyright (C) 2017 Alan Williams <mralert@gmail.com>
 * 
 * Portions taken from SDL 1.2.15
 * Copyright (C) 1997-2012 Sam Latinga <slouken@libsdl.org>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "SDL2.h"
#include "video.h"

typedef struct SDL1_Overlay {
	Uint32 format;
	int w, h;
	int planes;
	Uint16 *pitches;
	Uint8 **pixels;

	/* Originally struct private_yuvhwfuncs *hwfuncs; */
	void *pad;
	/* Originally struct private_yuvhwdata *hwdata; */
	SDL_Texture *texture;

	Uint32 hw_overlay :1;
	Uint32 UnusedBits :31;
} SDL1_Overlay;

#define SDL1_YV12_OVERLAY  0x32315659
#define SDL1_IYUV_OVERLAY  0x56555949
#define SDL1_YUY2_OVERLAY  0x32595559
#define SDL1_UYVY_OVERLAY  0x59565955
#define SDL1_YVYU_OVERLAY  0x55595659

static int align_num (int x, int a) {
	return ((x + a - 1) / a) * a;
}

DECLSPEC SDL1_Overlay *SDLCALL SDL_CreateYUVOverlay (int width, int height, Uint32 format, SDL1_Surface *display) {
	SDL1_Overlay *overlay;
	Uint16 pitches[3];
	int sizes[3];
	int planes, size, i;
	Uint32 texfmt;
	SDL_Texture *texture;
	int whalf = (width + 1) / 2;
	int hhalf = (height + 1) / 2;
	Uint8 *pixels;
	if (!SDLCL_renderer) return NULL;
	/* TODO: Support software overlays for other surfaces */
	if (display != SDLCL_surface) return NULL;
	switch (format) {
		case SDL1_YV12_OVERLAY:
		case SDL1_IYUV_OVERLAY:
			planes = 3;
			pitches[0] = align_num(width, 8);
			pitches[1] = align_num(whalf, 8);
			pitches[2] = align_num(whalf, 8);
			sizes[0] = pitches[0] * height;
			sizes[1] = pitches[1] * hhalf;
			sizes[2] = pitches[2] * hhalf;
			break;
		case SDL1_YUY2_OVERLAY:
		case SDL1_UYVY_OVERLAY:
		case SDL1_YVYU_OVERLAY:
			planes = 1;
			pitches[0] = align_num(width * 2, 8);
			sizes[0] = pitches[0] * height;
			break;
		default:
			return NULL;
	}
	for (i = 0, size = 0; i < planes; i++) size += sizes[i];
	switch (format) {
		case SDL1_YV12_OVERLAY: texfmt = SDL_PIXELFORMAT_YV12; break;
		case SDL1_IYUV_OVERLAY: texfmt = SDL_PIXELFORMAT_IYUV; break;
		case SDL1_YUY2_OVERLAY: texfmt = SDL_PIXELFORMAT_YUY2; break;
		case SDL1_UYVY_OVERLAY: texfmt = SDL_PIXELFORMAT_UYVY; break;
		case SDL1_YVYU_OVERLAY: texfmt = SDL_PIXELFORMAT_YVYU; break;
		default: return NULL;
	}
	overlay = malloc(sizeof(SDL1_Overlay) + sizeof(Uint8 *) * planes + sizeof(Uint16) * planes);
	if (!overlay) return NULL;
	pixels = malloc(size);
	if (!pixels) {
		free(overlay);
		return NULL;
	}
	texture = rSDL_CreateTexture(SDLCL_renderer, texfmt, SDL_TEXTUREACCESS_STREAMING, width, height);
	if (!texture) {
		free(pixels);
		free(overlay);
		return NULL;
	}
	overlay->format = format;
	overlay->w = width;
	overlay->h = height;
	overlay->planes = planes;
	overlay->pixels = (Uint8 **)(overlay + 1);
	overlay->pitches = (Uint16 *)(overlay->pixels + planes);
	overlay->pixels[0] = pixels;
	for (i = 1; i < planes; i++) overlay->pixels[i] = overlay->pixels[i - 1] + sizes[i - 1];
	memcpy(overlay->pitches, pitches, sizeof(Uint16) * planes);
	overlay->texture = texture;
	overlay->hw_overlay = 1;
	return overlay;
}

DECLSPEC int SDLCALL SDL_LockYUVOverlay (SDL1_Overlay *overlay) {
	(void)overlay;
	return 0;
}

DECLSPEC void SDLCALL SDL_UnlockYUVOverlay (SDL1_Overlay *overlay) {
	(void)overlay;
}

static int scale_x (int x) {
	return x * SDLCL_scale_rect.w / SDLCL_virtual_width + SDLCL_scale_rect.x;
}

static int scale_y (int y) {
	return y * SDLCL_scale_rect.h / SDLCL_virtual_height + SDLCL_scale_rect.y;
}

DECLSPEC int SDLCALL SDL_DisplayYUVOverlay (SDL1_Overlay *overlay, SDL1_Rect *dstrect) {
	SDL_Rect dest;
	if (overlay->planes == 3) {
		if (overlay->format == SDL1_YV12_OVERLAY)
			rSDL_UpdateYUVTexture(overlay->texture, NULL,
				overlay->pixels[0], overlay->pitches[0],
				overlay->pixels[2], overlay->pitches[2],
				overlay->pixels[1], overlay->pitches[1]);
		else
			rSDL_UpdateYUVTexture(overlay->texture, NULL,
				overlay->pixels[0], overlay->pitches[0],
				overlay->pixels[1], overlay->pitches[1],
				overlay->pixels[2], overlay->pitches[2]);
	} else
		rSDL_UpdateTexture(overlay->texture, NULL, overlay->pixels[0], overlay->pitches[0]);
	rSDL_RenderClear(SDLCL_renderer);
	if (SDLCL_scaling) {
		dest.x = scale_x(dstrect->x);
		dest.y = scale_y(dstrect->y);
		dest.w = scale_x(dstrect->x + dstrect->w) - dest.x;
		dest.h = scale_y(dstrect->y + dstrect->h) - dest.y;
	} else {
		dest.x = dstrect->x;
		dest.y = dstrect->y;
		dest.w = dstrect->w;
		dest.h = dstrect->h;
	}
	rSDL_RenderCopy(SDLCL_renderer, overlay->texture, NULL, &dest);
	rSDL_RenderPresent(SDLCL_renderer);
	return 0;
}

DECLSPEC void SDLCALL SDL_FreeYUVOverlay (SDL1_Overlay *overlay) {
	if (overlay) {
		free(overlay->pixels[0]);
		rSDL_DestroyTexture(overlay->texture);
		free(overlay);
	}
}
