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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "SDL2.h"
#include "SDL_opengl.h"
#include "video.h"
#include "events.h"
#include "rwops.h"
#include "version.h"
#include "loadso.h"

/* TODO: Proper initialization */
DECLSPEC int SDLCALL SDL_VideoInit (const char *driver_name, Uint32 flags) {
	(void)flags;
	return rSDL_VideoInit(driver_name);
}

static void close_window (void);

DECLSPEC void SDLCALL SDL_VideoQuit (void) {
	close_window();
	rSDL_VideoQuit();
}

typedef struct SDL1_Proxy {
	SDL1_Surface surface;
	SDL1_PixelFormat format;
	SDL1_Palette palette;
	SDL1_Color colors[1];
} SDL1_Proxy;

/* SDL_CreateRGBSurface() & SDL_SetVideoMode() flags */
#define SDL1_SWSURFACE   0x00000000
#define SDL1_HWSURFACE   0x00000001
#define SDL1_ASYNCBLIT   0x00000004

/* SDL_SetVideoMode() flags */
#define SDL1_ANYFORMAT   0x10000000
#define SDL1_HWPALETTE   0x20000000
#define SDL1_DOUBLEBUF   0x40000000
#define SDL1_FULLSCREEN  0x80000000
#define SDL1_OPENGL      0x00000002
#define SDL1_OPENGLBLIT  0x0000000A
#define SDL1_RESIZABLE   0x00000010
#define SDL1_NOFRAME     0x00000020

/* SDL1_Surface flags */
#define SDL1_HWACCEL     0x00000100
#define SDL1_SRCCOLORKEY 0x00001000
#define SDL1_RLEACCELOK  0x00002000
#define SDL1_RLEACCEL    0x00004000
#define SDL1_SRCALPHA    0x00010000
#define SDL1_PREALLOC    0x01000000

static Uint8 masktoloss (Uint32 mask) {
	Uint8 loss = 8;
	if (!mask) return 0;
	while (!(mask & 1)) mask >>= 1;
	while (mask & 1) {
		mask >>= 1;
		loss--;
	}
	return loss;
}

static Uint8 masktoshift (Uint32 mask) {
	Uint8 shift = 0;
	if (!mask) return 0;
	while (!(mask & 1)) {
		mask >>= 1;
		shift++;
	}
	return shift;
}

static void process_masks (SDL1_PixelFormat *format) {
	format->Rloss = masktoloss(format->Rmask);
	format->Gloss = masktoloss(format->Gmask);
	format->Bloss = masktoloss(format->Bmask);
	format->Aloss = masktoloss(format->Amask);
	format->Rshift = masktoshift(format->Rmask);
	format->Gshift = masktoshift(format->Gmask);
	format->Bshift = masktoshift(format->Bmask);
	format->Ashift = masktoshift(format->Amask);
}

static SDL1_Surface *SDLCL_CreateSurfaceFromSDL2(SDL_Surface *surface2) {
	SDL1_Proxy *proxy;
	int i, ncolors = surface2->format->palette ? surface2->format->palette->ncolors : 0;
	SDL_Color *color;
	SDL_BlendMode blend;
	proxy = malloc(sizeof(SDL1_Proxy) + ncolors * sizeof(SDL1_Color));
	if (!proxy) {
		return NULL;
	}
	proxy->surface.flags = 0;
	proxy->surface.format = &proxy->format;
	proxy->surface.w = surface2->w;
	proxy->surface.h = surface2->h;
	proxy->surface.pitch = surface2->pitch;
	proxy->surface.pixels = surface2->pixels;
	proxy->surface.offset = SDL_MUSTLOCK(surface2);
	proxy->surface.sdl2_surface = surface2;
	proxy->surface.clip_rect.x = surface2->clip_rect.x;
	proxy->surface.clip_rect.y = surface2->clip_rect.y;
	proxy->surface.clip_rect.w = surface2->clip_rect.w;
	proxy->surface.clip_rect.h = surface2->clip_rect.h;
	proxy->surface.refcount = 1;
	proxy->format.palette = surface2->format->palette ? &proxy->palette : NULL;
	if (proxy->format.palette) {
		proxy->palette.ncolors = ncolors;
		proxy->palette.colors = proxy->colors;
		for (i = 0; i < ncolors; i++) {
			color = surface2->format->palette->colors + i;
			proxy->colors[i].r = color->r;
			proxy->colors[i].g = color->g;
			proxy->colors[i].b = color->b;
			proxy->colors[i].unused = 0;
		}
	}
	proxy->format.BitsPerPixel = surface2->format->BitsPerPixel;
	proxy->format.BytesPerPixel = surface2->format->BytesPerPixel;
	proxy->format.Rmask = surface2->format->Rmask;
	proxy->format.Gmask = surface2->format->Gmask;
	proxy->format.Bmask = surface2->format->Bmask;
	proxy->format.Amask = surface2->format->Amask;
	process_masks(&proxy->format);
	if (rSDL_GetColorKey(surface2, &proxy->format.colorkey))
		proxy->format.colorkey = 0;
	else
		proxy->surface.flags |= SDL1_SRCCOLORKEY;
	if (rSDL_GetSurfaceAlphaMod(surface2, &proxy->format.alpha))
		proxy->format.alpha = 255;
	if (!rSDL_GetSurfaceBlendMode(surface2, &blend) && blend == SDL_BLENDMODE_BLEND)
		proxy->surface.flags |= SDL1_SRCALPHA;
	return &proxy->surface;
}

static void update_surface_blend (SDL1_Surface *surface) {
	int isalpha = surface->format->Amask && !surface->format->palette;
	int colorkey = (surface->flags & SDL1_SRCCOLORKEY) && (!isalpha || !(surface->flags & SDL1_SRCALPHA));
	int alphablend = surface->flags & SDL1_SRCALPHA && (isalpha || surface->format->alpha != 255);
	rSDL_SetSurfaceRLE(surface->sdl2_surface, surface->flags & SDL1_RLEACCEL);
	rSDL_SetSurfaceBlendMode(surface->sdl2_surface, alphablend ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
	rSDL_SetSurfaceAlphaMod(surface->sdl2_surface, (isalpha || !alphablend) ? 255 : surface->format->alpha);
	rSDL_SetColorKey(surface->sdl2_surface, colorkey ? SDL_TRUE : SDL_FALSE, surface->format->colorkey);
	surface->offset = SDL_MUSTLOCK(surface->sdl2_surface);
}

DECLSPEC SDL1_Surface *SDLCALL SDL_CreateRGBSurface (Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
	SDL_Surface *surface2;
	SDL1_Surface *surface;
	if (Amask) flags |= SDL1_SRCALPHA;
	surface2 = rSDL_CreateRGBSurface(0, width, height, depth, Rmask, Gmask, Bmask, Amask);
	if (!surface2) return NULL;
	surface = SDLCL_CreateSurfaceFromSDL2(surface2);
	if (!surface) {
		rSDL_FreeSurface(surface2);
		return NULL;
	}
	surface->flags |= flags & (SDL1_SRCCOLORKEY | SDL1_SRCALPHA);
	update_surface_blend(surface);
	return surface;
}

DECLSPEC SDL1_Surface *SDLCALL SDL_CreateRGBSurfaceFrom (void *pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
	SDL_Surface *surface2;
	SDL1_Surface *surface;
	surface2 = rSDL_CreateRGBSurfaceFrom(pixels, width, height, depth, pitch, Rmask, Gmask, Bmask, Amask);
	if (!surface2) return NULL;
	surface = SDLCL_CreateSurfaceFromSDL2(surface2);
	if (!surface) {
		rSDL_FreeSurface(surface2);
		return NULL;
	}
	return surface;
}

DECLSPEC int SDLCALL SDL_SetAlpha (SDL1_Surface *surface, Uint32 flag, Uint8 alpha) {
	surface->flags = (surface->flags & ~(SDL1_SRCALPHA | SDL1_RLEACCEL)) | (flag & (SDL1_SRCALPHA | SDL1_RLEACCEL));
	surface->format->alpha = alpha;
	update_surface_blend(surface);
	return 0;
}

DECLSPEC int SDLCALL SDL_SetColorKey (SDL1_Surface *surface, Uint32 flag, Uint32 key) {
	surface->flags = (surface->flags & ~(SDL1_SRCCOLORKEY | SDL1_RLEACCEL)) | (flag & (SDL1_SRCCOLORKEY | SDL1_RLEACCEL));
	surface->format->colorkey = key;
	update_surface_blend(surface);
	return 0;
}

DECLSPEC void SDL_GetClipRect (SDL1_Surface *surface, SDL1_Rect *rect) {
	*rect = surface->clip_rect;
}

DECLSPEC void SDL_SetClipRect (SDL1_Surface *surface, SDL1_Rect *rect) {
	SDL_Rect rect2, *rectptr = NULL;
	if (rect) {
		rect2.x = rect->x;
		rect2.y = rect->y;
		rect2.w = rect->w;
		rect2.h = rect->h;
		rectptr = &rect2;
	}
	rSDL_SetClipRect(surface->sdl2_surface, rectptr);
	rSDL_GetClipRect(surface->sdl2_surface, &rect2);
	surface->clip_rect.x = rect2.x;
	surface->clip_rect.y = rect2.y;
	surface->clip_rect.w = rect2.w;
	surface->clip_rect.h = rect2.h;
}

DECLSPEC int SDLCALL SDL_FillRect (SDL1_Surface *dst, SDL1_Rect *dstrect, Uint32 color) {
	SDL_Rect rect2, *rectptr = NULL;
	if (dstrect) {
		rect2.x = dstrect->x;
		rect2.y = dstrect->y;
		rect2.w = dstrect->w;
		rect2.h = dstrect->h;
		rectptr = &rect2;
	}
	return rSDL_FillRect(dst->sdl2_surface, rectptr, color);
}

/* a.k.a. SDL_BlitSurface() */
DECLSPEC int SDLCALL SDL_UpperBlit (SDL1_Surface *src, SDL1_Rect *srcrect, SDL1_Surface *dst, SDL1_Rect *dstrect) {
	SDL_Rect srcrect2, *srcptr = NULL;
	SDL_Rect dstrect2, *dstptr = NULL;
	int ret;
	if (!src || !dst) return -1;
	if (srcrect) {
		srcrect2.x = srcrect->x;
		srcrect2.y = srcrect->y;
		srcrect2.w = srcrect->w;
		srcrect2.h = srcrect->h;
		srcptr = &srcrect2;
	}
	if (dstrect) {
		dstrect2.x = dstrect->x;
		dstrect2.y = dstrect->y;
		dstrect2.w = dstrect->w;
		dstrect2.h = dstrect->h;
		dstptr = &dstrect2;
	}
	ret = rSDL_UpperBlit(src->sdl2_surface, srcptr, dst->sdl2_surface, dstptr);
	if (dstrect) {
		dstrect->x = dstrect2.x;
		dstrect->y = dstrect2.y;
		dstrect->w = dstrect2.w;
		dstrect->h = dstrect2.h;
	}
	return ret;
}

DECLSPEC int SDLCALL SDL_LowerBlit (SDL1_Surface *src, SDL1_Rect *srcrect, SDL1_Surface *dst, SDL1_Rect *dstrect) {
	SDL_Rect srcrect2, *srcptr = NULL;
	SDL_Rect dstrect2;
	if (srcrect) {
		srcrect2.x = srcrect->x;
		srcrect2.y = srcrect->y;
		srcrect2.w = srcrect->w;
		srcrect2.h = srcrect->h;
		srcptr = &srcrect2;
	}
	dstrect2.x = dstrect->x;
	dstrect2.y = dstrect->y;
	dstrect2.w = dstrect->w;
	dstrect2.h = dstrect->h;
	return rSDL_LowerBlit(src->sdl2_surface, srcptr, dst->sdl2_surface, &dstrect2);
}

DECLSPEC int SDLCALL SDL_LockSurface (SDL1_Surface *surface) {
	int ret = rSDL_LockSurface(surface->sdl2_surface);
	surface->pixels = surface->sdl2_surface->pixels;
	return ret;
}

DECLSPEC void SDLCALL SDL_UnlockSurface (SDL1_Surface *surface) {
	rSDL_UnlockSurface(surface->sdl2_surface);
}

DECLSPEC SDL1_Surface *SDLCALL SDL_LoadBMP_RW (SDL1_RWops *src, int freesrc) {
	SDL1_Surface *surface;
	SDL_Surface *surface2;
	SDL_RWops *src2 = SDLCL_RWFromSDL1(src);
	if (!src2) {
		if (freesrc && src) SDL1_RWclose(src);
		return NULL;
	}
	surface2 = rSDL_LoadBMP_RW(src2, freesrc);
	if (!freesrc) rSDL_FreeRW(src2);
	if (!surface2) return NULL;
	surface = SDLCL_CreateSurfaceFromSDL2(surface2);
	if (!surface) {
		rSDL_FreeSurface(surface2);
		return NULL;
	}
	return surface;
}

DECLSPEC int SDLCALL SDL_SaveBMP_RW (SDL1_Surface *surface, SDL1_RWops *dst, int freedst) {
	int ret;
	SDL_RWops *dst2 = SDLCL_RWFromSDL1(dst);
	if (!dst2) return -1;
	ret = rSDL_SaveBMP_RW(surface->sdl2_surface, dst2, freedst);
	if (!freedst) rSDL_FreeRW(dst2);
	return ret;
}

static Uint32 map_component (Uint8 v, Uint8 loss, Uint8 shift, Uint32 mask) {
	return (((Uint32)v >> loss) << shift) & mask;
}

DECLSPEC Uint32 SDLCALL SDL_MapRGBA (SDL1_PixelFormat *fmt, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	int i, dist, min, rd, gd, bd;
	Uint32 color;
	if (fmt->palette) {
		min = (256 * 256) * 3;
		color = 0;
		for (i = 0; i < fmt->palette->ncolors; i++) {
			rd = fmt->palette->colors[i].r - r;
			gd = fmt->palette->colors[i].g - g;
			bd = fmt->palette->colors[i].b - b;
			dist = (rd * rd) + (gd * gd) + (bd * bd);
			if (dist < min) {
				color = i;
				if (dist == 0) break;
				min = dist;
			}
		}
		return color;
	} else {
		color = map_component(r, fmt->Rloss, fmt->Rshift, fmt->Rmask);
		color |= map_component(g, fmt->Gloss, fmt->Gshift, fmt->Gmask);
		color |= map_component(b, fmt->Bloss, fmt->Bshift, fmt->Bmask);
		color |= map_component(a, fmt->Aloss, fmt->Ashift, fmt->Amask);
		return color;
	}
}

DECLSPEC Uint32 SDLCALL SDL_MapRGB (SDL1_PixelFormat *fmt, Uint8 r, Uint8 g, Uint8 b) {
	return SDL_MapRGBA(fmt, r, g, b, 255);
}

static Uint8 get_component (Uint32 pixel, Uint8 loss, Uint8 shift, Uint32 mask) {
	Uint8 v = ((pixel & mask) >> shift) << loss;
	int s = 8 - loss;
	while (s < 8) {
		v |= v >> s;
		s *= 2;
	}
	return v;
}

DECLSPEC void SDLCALL SDL_GetRGBA (Uint32 pixel, SDL1_PixelFormat *fmt, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a) {
	if (fmt->palette) {
		*r = fmt->palette->colors[pixel].r;
		*g = fmt->palette->colors[pixel].g;
		*b = fmt->palette->colors[pixel].b;
		*a = 255;
	} else {
		*r = get_component(pixel, fmt->Rloss, fmt->Rshift, fmt->Rmask);
		*g = get_component(pixel, fmt->Gloss, fmt->Gshift, fmt->Gmask);
		*b = get_component(pixel, fmt->Bloss, fmt->Bshift, fmt->Bmask);
		if (fmt->Amask) *a = get_component(pixel, fmt->Aloss, fmt->Ashift, fmt->Amask);
		else *a = 255;
	}
}

DECLSPEC void SDLCALL SDL_GetRGB (Uint32 pixel, SDL1_PixelFormat *fmt, Uint8 *r, Uint8 *g, Uint8 *b) {
	Uint8 a;
	SDL_GetRGBA(pixel, fmt, r, g, b, &a);
}

typedef struct SDL1_VideoInfo {
	Uint32 hw_available :1;
	Uint32 wm_available :1;
	Uint32 UnusedBits1  :6;
	Uint32 UnusedBits2  :1;
	Uint32 blit_hw      :1;
	Uint32 blit_hw_CC   :1;
	Uint32 blit_hw_A    :1;
	Uint32 blit_sw      :1;
	Uint32 blit_sw_CC   :1;
	Uint32 blit_sw_A    :1;
	Uint32 blit_fill    :1;
	Uint32 UnusedBits3  :16;
	Uint32 video_mem;
	SDL1_PixelFormat *vfmt;
	int current_w;
	int current_h;
} SDL1_VideoInfo;

static SDL1_PixelFormat video_fmt = {
	NULL, /* palette */
	32, /* BitsPerPixel */
	4, /* BytesPerPixel */
	0, 0, 0, 0, /* [RGBA]loss */
	0, 8, 16, 24, /* [RGBA]shift */
	0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000, /* [RGBA]mask */
	0, 0 /* colorkey, alpha */
};

static SDL1_VideoInfo video_info = {
	0,          /* hw_available */
	1,          /* wm_available */
	0,          /* UnusedBits1 */
	0,          /* UnusedBits2 */
	0,          /* blit_hw */
	0,          /* blit_hw_CC */
	0,          /* blit_hw_A */
	0,          /* blit_sw */
	0,          /* blit_sw_CC */
	0,          /* blit_sw_A */
	0,          /* blit_fill */
	0,          /* UnusedBits3 */
	262144,     /* video_mem */
	&video_fmt, /* vfmt */
	640,        /* current_w */
	480         /* current_h */
};

DECLSPEC SDL1_VideoInfo *SDLCALL SDL_GetVideoInfo (void) {
	return &video_info;
}

DECLSPEC char *SDLCALL SDL_VideoDriverName (char *namebuf, int maxlen) {
	const char *name = rSDL_GetCurrentVideoDriver();
	if (!name) return NULL;
	if (maxlen) {
		strncpy(namebuf, name, maxlen);
		namebuf[maxlen - 1] = 0;
	}
	return namebuf;
}

#define SDL_WINDOW_INVALID ((Uint32)-1)

static Uint32 vidflags1to2 (Uint32 flags) {
	Uint32 flags2 = 0;
	/* No OPENGLBLIT support */
	if (flags & SDL1_OPENGLBLIT & ~SDL1_OPENGL) return SDL_WINDOW_INVALID;
	if (flags & SDL1_FULLSCREEN) flags2 |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	if (flags & SDL1_OPENGL) flags2 |= SDL_WINDOW_OPENGL;
	/* TODO: Figure out how to do SDL 1.2 resizable window interface with SDL 2.0 */
	/* if (flags & SDL1_RESIZABLE) flags2 |= SDL_WINDOW_RESIZABLE; */
	if (flags & SDL1_NOFRAME) flags2 |= SDL_WINDOW_BORDERLESS;
	return flags2;
}

static SDL1_Rect **mode_list = NULL;

static void free_mode_list (void) {
	int i;
	if (mode_list) {
		for (i = 0; mode_list[i]; i++)
			free(mode_list[i]);
		free(mode_list);
		mode_list = NULL;
	}
}

static int rect_equal (SDL1_Rect *a, SDL1_Rect *b) {
	if (a->x != b->x) return 0;
	if (a->y != b->y) return 0;
	if (a->w != b->w) return 0;
	if (a->h != b->h) return 0;
	return 1;
}

DECLSPEC SDL1_Rect **SDLCALL SDL_ListModes (SDL1_PixelFormat *format, Uint32 flags) {
	(void)format;
	int i, j, nummode = rSDL_GetNumDisplayModes(0);
	SDL_DisplayMode mode;
	SDL1_Rect **ptr;
	if (vidflags1to2(flags) == SDL_WINDOW_INVALID) return NULL;
	if (!(flags & SDL1_FULLSCREEN)) return (SDL1_Rect **)-1;
	if (mode_list) return mode_list;
	mode_list = malloc(sizeof(SDL_Rect *) * (nummode + 1));
	for (i = 0; i < nummode + 1; i++)
		mode_list[i] = NULL;
	ptr = mode_list;
	for (i = 0; i < nummode; i++) {
		if (!rSDL_GetDisplayMode(0, i, &mode)) {
			*ptr = malloc(sizeof(SDL_Rect));
			if (!*ptr) {
				free_mode_list();
				return NULL;
			}
			(*ptr)->x = (*ptr)->y = 0;
			(*ptr)->w = mode.w;
			(*ptr)->h = mode.h;
			ptr++;
		}
	}
	for (i = 1, j = 0; mode_list[i]; i++) {
		if (rect_equal(mode_list[i], mode_list[j])) free(mode_list[i]);
		else mode_list[++j] = mode_list[i];
		if (i != j) mode_list[i] = NULL;
	}
	return mode_list;
}

DECLSPEC int SDLCALL SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags) {
	int numdisp = rSDL_GetNumVideoDisplays();
	int i, j, nummode, tbpp, cbpp = 0;
	SDL_DisplayMode mode;
	if (vidflags1to2(flags) == SDL_WINDOW_INVALID) return 0;
	if (bpp < 8 || bpp > 32) return 0;
	if (width <= 0 || height <= 0) return 0;
	if (!(flags & SDL1_FULLSCREEN)) {
		if (!rSDL_GetDesktopDisplayMode(0, &mode))
			return SDL_BITSPERPIXEL(mode.format);
		else
			return 0;
	}
	for (i = 0; i < numdisp; i++) {
		nummode = rSDL_GetNumDisplayModes(i);
		for (j = 0; j < nummode; j++) {
			if (!rSDL_GetDisplayMode(i, j, &mode)) {
				if (mode.w >= width && mode.h >= height) {
					tbpp = SDL_BITSPERPIXEL(mode.format);
					if (cbpp / 8 != bpp / 8 || tbpp / 8 == bpp / 8) {
						/* Try to match byte-per-pixel count first */
						if (cbpp / 8 != bpp / 8 && tbpp / 8 == bpp / 8) cbpp = tbpp;
						/* Then try higher bit depth */
						if (cbpp < bpp && tbpp > cbpp) cbpp = tbpp;
						/* Then try lowest bit depth higher than or equal to requested */
						if (cbpp > bpp && tbpp >= bpp && tbpp < cbpp) cbpp = tbpp;
					}
				}
			}
		}
	}
	return cbpp;
}

SDL_Window *SDLCL_window = NULL;
SDL_Renderer *SDLCL_renderer = NULL;
static SDL_Texture *main_texture = NULL;
static SDL_GLContext main_glcontext = NULL;
SDL1_Surface *SDLCL_surface = NULL;

static SDL_GLContext scale_glcontext = NULL;

static SDL1_PixelFormat texture_format;
static Uint32 physical_palette[256];

static Uint32 mode_flags = 0;
static SDL_bool grab = SDL_FALSE;

static char *window_title = NULL;
static char *window_icon = NULL;

int SDLCL_scaling = 0;
int SDLCL_virtual_width, SDLCL_virtual_height;
static int real_width, real_height;
SDL_Rect SDLCL_scale_rect;

DECLSPEC void SDLCALL SDL_FreeSurface (SDL1_Surface *surface) {
	SDL1_Proxy *proxy;
	if (surface && surface != SDLCL_surface) {
		proxy = (SDL1_Proxy *)surface;
		rSDL_FreeSurface(surface->sdl2_surface);
		free(proxy);
	}
}

static void close_window (void) {
	SDL1_Surface *surface;
	if (SDLCL_surface) {
		surface = SDLCL_surface;
		SDLCL_surface = NULL;
		SDL_FreeSurface(surface);
	}
	if (scale_glcontext) {
		rSDL_GL_DeleteContext(scale_glcontext);
		scale_glcontext = NULL;
	}
	if (main_glcontext) {
		rSDL_GL_DeleteContext(main_glcontext);
		main_glcontext = NULL;
	}
	if (main_texture) {
		rSDL_DestroyTexture(main_texture);
		main_texture = NULL;
	}
	if (SDLCL_renderer) {
		rSDL_DestroyRenderer(SDLCL_renderer);
		SDLCL_renderer = NULL;
	}
	if (SDLCL_window) {
		rSDL_DestroyWindow(SDLCL_window);
		SDLCL_window = NULL;
	}
}

void SDLCL_UpdateGrab (void) {
	int showing = SDL_ShowCursor(SDL1_QUERY);
	rSDL_ShowCursor((!SDLCL_scaling && showing) ? SDL_ENABLE : SDL_DISABLE);
	if (SDLCL_window)
		rSDL_SetWindowGrab(SDLCL_window, (mode_flags & SDL1_FULLSCREEN) ? SDL_TRUE : grab);
	if (/*grab &&*/ !showing)
		rSDL_SetRelativeMouseMode(SDL_TRUE);
	else
		rSDL_SetRelativeMouseMode(SDL_FALSE);
}

static int next_pow2 (int x) {
	int nx = x;
	int s = 1;
	do {
		x = nx;
		nx |= nx >> s;
		s *= 2;
	} while (nx != x);
	return x + 1;
}

static void (APIENTRY *main_glFinish)(void);
static void (APIENTRY *scale_glBitmap)(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
static void (APIENTRY *scale_glClear)(GLbitfield mask);
static void (APIENTRY *scale_glCopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
static void (APIENTRY *scale_glDisable)(GLenum cap);
static void (APIENTRY *scale_glDrawArrays)(GLenum mode, GLint first, GLsizei count);
static void (APIENTRY *scale_glDrawPixels)(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *data);
static void (APIENTRY *scale_glEnable)(GLenum cap);
static void (APIENTRY *scale_glFinish)(void);
static void (APIENTRY *scale_glRasterPos2f)(GLfloat x, GLfloat y);
static void (APIENTRY *scale_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);

static GLfloat scale_vertex[6] = { -1, -1, 3, -1, -1, 3 };
static GLfloat scale_texcoord[6] = { 0, 0, 2, 0, 0, 2 };

static int init_scale (void) {
	void (APIENTRY *scale_glBindTexture)(GLenum target, GLuint texture);
	void (APIENTRY *scale_glBlendFunc)(GLenum sfactor, GLenum dfactor);
	void (APIENTRY *scale_glEnableClientState)(GLenum cap);
	void (APIENTRY *scale_glGenTextures)(GLsizei n, GLuint *textures);
	void (APIENTRY *scale_glLoadIdentity)(void);
	void (APIENTRY *scale_glMatrixMode)(GLenum mode);
	void (APIENTRY *scale_glPixelZoom)(GLfloat xfactor, GLfloat yfactor);
	void (APIENTRY *scale_glTexCoordPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
	void (APIENTRY *scale_glTexImage2D)(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
	void (APIENTRY *scale_glTexParameteri)(GLenum target, GLenum pname, GLint param);
	void (APIENTRY *scale_glVertexPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
	void (APIENTRY *main_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
	int texw, texh;
	GLuint scale_texture;
	void *texp;
	SDLCL_scale_rect.w = (SDLCL_surface->w * real_height) / SDLCL_surface->h;
	SDLCL_scale_rect.h = (SDLCL_surface->h * real_width) / SDLCL_surface->w;
	if (SDLCL_scale_rect.w > real_width) SDLCL_scale_rect.w = real_width;
	else SDLCL_scale_rect.h = real_height;
	SDLCL_scale_rect.x = (real_width - SDLCL_scale_rect.w) / 2;
	SDLCL_scale_rect.y = (real_height - SDLCL_scale_rect.h) / 2;
	if (mode_flags & SDL1_OPENGL) {
		texw = next_pow2(SDLCL_surface->w);
		texh = next_pow2(SDLCL_surface->h);
		scale_texcoord[2] = (GLfloat)SDLCL_surface->w / (GLfloat)texw * 2.0;
		scale_texcoord[5] = (GLfloat)SDLCL_surface->h / (GLfloat)texh * 2.0;
		scale_glcontext = rSDL_GL_CreateContext(SDLCL_window);
		if (!scale_glcontext) return 0;
		scale_glBitmap = rSDL_GL_GetProcAddress("glBitmap");
		if (!scale_glBitmap) return 0;
		scale_glBindTexture = rSDL_GL_GetProcAddress("glBindTexture");
		if (!scale_glBindTexture) return 0;
		scale_glBlendFunc = rSDL_GL_GetProcAddress("glBlendFunc");
		if (!scale_glBlendFunc) return 0;
		scale_glClear = rSDL_GL_GetProcAddress("glClear");
		if (!scale_glClear) return 0;
		scale_glCopyTexSubImage2D = rSDL_GL_GetProcAddress("glCopyTexSubImage2D");
		if (!scale_glCopyTexSubImage2D) return 0;
		scale_glDisable = rSDL_GL_GetProcAddress("glDisable");
		if (!scale_glDisable) return 0;
		scale_glDrawArrays = rSDL_GL_GetProcAddress("glDrawArrays");
		if (!scale_glDrawArrays) return 0;
		scale_glDrawPixels = rSDL_GL_GetProcAddress("glDrawPixels");
		if (!scale_glDrawPixels) return 0;
		scale_glEnable = rSDL_GL_GetProcAddress("glEnable");
		if (!scale_glEnable) return 0;
		scale_glEnableClientState = rSDL_GL_GetProcAddress("glEnableClientState");
		if (!scale_glEnableClientState) return 0;
		scale_glFinish = rSDL_GL_GetProcAddress("glFinish");
		if (!scale_glFinish) return 0;
		scale_glGenTextures = rSDL_GL_GetProcAddress("glGenTextures");
		if (!scale_glGenTextures) return 0;
		scale_glLoadIdentity = rSDL_GL_GetProcAddress("glLoadIdentity");
		if (!scale_glLoadIdentity) return 0;
		scale_glMatrixMode = rSDL_GL_GetProcAddress("glMatrixMode");
		if (!scale_glMatrixMode) return 0;
		scale_glPixelZoom = rSDL_GL_GetProcAddress("glPixelZoom");
		if (!scale_glPixelZoom) return 0;
		scale_glRasterPos2f = rSDL_GL_GetProcAddress("glRasterPos2f");
		if (!scale_glRasterPos2f) return 0;
		scale_glTexCoordPointer = rSDL_GL_GetProcAddress("glTexCoordPointer");
		if (!scale_glTexCoordPointer) return 0;
		scale_glTexImage2D = rSDL_GL_GetProcAddress("glTexImage2D");
		if (!scale_glTexImage2D) return 0;
		scale_glTexParameteri = rSDL_GL_GetProcAddress("glTexParameteri");
		if (!scale_glTexParameteri) return 0;
		scale_glVertexPointer = rSDL_GL_GetProcAddress("glVertexPointer");
		if (!scale_glVertexPointer) return 0;
		scale_glViewport = rSDL_GL_GetProcAddress("glViewport");
		if (!scale_glViewport) return 0;
		texp = calloc(texw * texh, 1);
		if (!texp) return 0;
		scale_glMatrixMode(GL_MODELVIEW);
		scale_glLoadIdentity();
		scale_glMatrixMode(GL_PROJECTION);
		scale_glLoadIdentity();
		scale_glMatrixMode(GL_TEXTURE);
		scale_glLoadIdentity();
		scale_glViewport(SDLCL_scale_rect.x, SDLCL_scale_rect.y, SDLCL_scale_rect.w, SDLCL_scale_rect.h);
		scale_glEnable(GL_TEXTURE_2D);
		scale_glGenTextures(1, &scale_texture);
		scale_glBindTexture(GL_TEXTURE_2D, scale_texture);
		scale_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, texw, texh, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, texp);
		scale_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		scale_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		scale_glVertexPointer(2, GL_FLOAT, 0, &scale_vertex);
		scale_glTexCoordPointer(2, GL_FLOAT, 0, &scale_texcoord);
		scale_glEnableClientState(GL_VERTEX_ARRAY);
		scale_glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		scale_glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		scale_glPixelZoom(1, -1);
		free(texp);
		rSDL_GL_MakeCurrent(SDLCL_window, main_glcontext);
		main_glFinish = rSDL_GL_GetProcAddress("glFinish");
		if (!main_glFinish) return 0;
		main_glViewport = rSDL_GL_GetProcAddress("glViewport");
		if (!main_glViewport) return 0;
		main_glViewport(0, 0, SDLCL_surface->w, SDLCL_surface->h);
	}
	return 1;
}

static void gl_scale (void) {
	SDL1_Cursor *cursor;
	int x, y;
	main_glFinish();
	rSDL_GL_MakeCurrent(SDLCL_window, scale_glcontext);
	if (SDL_ShowCursor(SDL1_QUERY)) {
		cursor = SDL_GetCursor();
		SDL_GetMouseState(&x, &y);
		scale_glViewport(0, 0, SDLCL_virtual_width, SDLCL_virtual_height);
		scale_glEnable(GL_BLEND);
		scale_glDisable(GL_TEXTURE_2D);
		scale_glRasterPos2f(-1, 1);
		scale_glBitmap(0, 0, 0, 0, x - cursor->hot_x, -(y - cursor->hot_y), NULL);
		scale_glDrawPixels(cursor->area.w, cursor->area.h, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, cursor->image);
		scale_glEnable(GL_TEXTURE_2D);
		scale_glDisable(GL_BLEND);
		scale_glViewport(SDLCL_scale_rect.x, SDLCL_scale_rect.y, SDLCL_scale_rect.w, SDLCL_scale_rect.h);
	}
	scale_glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, SDLCL_surface->w, SDLCL_surface->h);
	scale_glFinish();
	scale_glClear(GL_COLOR_BUFFER_BIT);
	scale_glDrawArrays(GL_TRIANGLES, 0, 3);
	scale_glFinish();
	rSDL_GL_MakeCurrent(SDLCL_window, main_glcontext);
}

DECLSPEC SDL1_Surface *SDLCALL SDL_SetVideoMode (int width, int height, int bpp, Uint32 flags) {
	Uint32 Rmask, Gmask, Bmask, Amask;
	Uint32 pixfmt = SDL_PIXELFORMAT_ARGB8888;
	Uint32 flags2 = vidflags1to2(flags);
	int tbpp;
	if (flags2 == SDL_WINDOW_INVALID) return NULL;
	if (bpp && !(flags & SDL1_ANYFORMAT)) {
		switch (bpp) {
			case 8: pixfmt = SDL_PIXELFORMAT_INDEX8; break;
			case 15: pixfmt = SDL_PIXELFORMAT_RGB555; break;
			case 16: pixfmt = SDL_PIXELFORMAT_RGB565; break;
			case 24: pixfmt = SDL_PIXELFORMAT_RGB888; break;
			case 32: pixfmt = SDL_PIXELFORMAT_ARGB8888; break;
			default: return NULL;
		}
	}
	rSDL_PixelFormatEnumToMasks(pixfmt, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
	Amask = 0;
	close_window();
	if (flags & SDL1_OPENGL) {
		/* Use compatibility profile for OpenGL */
		rSDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
		rSDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
		rSDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	}
	SDLCL_window = rSDL_CreateWindow(window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags2);
	if (!SDLCL_window) return NULL;
        rSDL_GetWindowSize(SDLCL_window, &real_width, &real_height);
	if (flags & SDL1_OPENGL) {
		main_glcontext = rSDL_GL_CreateContext(SDLCL_window);
		if (!main_glcontext) {
			close_window();
			return NULL;
		}
	} else {
		SDLCL_renderer = rSDL_CreateRenderer(SDLCL_window, -1, 0);
		if (!SDLCL_renderer) {
			close_window();
			return NULL;
		}
		rSDL_SetRenderDrawColor(SDLCL_renderer, 0, 0, 0, 255);
		rSDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
		if (bpp > 8) {
			main_texture = rSDL_CreateTexture(SDLCL_renderer, pixfmt, SDL_TEXTUREACCESS_STREAMING, width, height);
		} else {
			main_texture = rSDL_CreateTexture(SDLCL_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
			rSDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888,
				&tbpp,
				&texture_format.Rmask,
				&texture_format.Gmask,
				&texture_format.Bmask,
				&texture_format.Amask);
			texture_format.palette = NULL;
			texture_format.BitsPerPixel = tbpp;
			texture_format.BytesPerPixel = tbpp / 8;
			process_masks(&texture_format);
			memset(physical_palette, 0, sizeof(Uint32) * 256);
		}
		if (!main_texture) {
			close_window();
			return NULL;
		}
		rSDL_SetTextureBlendMode(main_texture, SDL_BLENDMODE_NONE);
	}
	SDLCL_surface = SDL_CreateRGBSurface(0, width, height, bpp, Rmask, Gmask, Bmask, Amask);
	if (!SDLCL_surface) {
		close_window();
		return NULL;
	}
	mode_flags = flags;
	SDLCL_scaling = (real_width != width || real_height != height);
	if (SDLCL_scaling) {
		SDLCL_virtual_width = width;
		SDLCL_virtual_height = height;
		if (!init_scale()) {
			close_window();
			return NULL;
		}
	}
	SDLCL_SetMouseRange(width, height);
	SDLCL_UpdateGrab();
	SDL_SetCursor(NULL);
	return SDLCL_surface;
}

DECLSPEC SDL1_Surface *SDLCALL SDL_GetVideoSurface (void) {
	return SDLCL_surface;
}

#define SDL1_LOGPAL  0x01
#define SDL1_PHYSPAL 0x02

DECLSPEC int SDLCALL SDL_SetPalette (SDL1_Surface *surface, int flags, SDL1_Color *colors, int firstcolor, int ncolors) {
	SDL_Color colors2[256];
	int i;
	if (ncolors > 256) return 0;
	if (flags & SDL1_LOGPAL) {
		for (i = 0; i < ncolors; i++) {
			colors2[i].r = colors[i].r;
			colors2[i].g = colors[i].g;
			colors2[i].b = colors[i].b;
			colors2[i].a = 255;
		}
		if (rSDL_SetPaletteColors(surface->sdl2_surface->format->palette, colors2, firstcolor, ncolors)) return 0;
		memcpy(surface->format->palette->colors + firstcolor, colors, ncolors * sizeof(SDL1_Color));
	}
	if ((flags & SDL1_PHYSPAL) && surface == SDLCL_surface) {
		if (firstcolor > 256 - ncolors) return 0;
		for (i = 0; i < ncolors; i++)
			physical_palette[firstcolor + i] = SDL_MapRGB(&texture_format, colors[i].r, colors[i].g, colors[i].b);
	}
	return 1;
}

DECLSPEC int SDLCALL SDL_SetColors (SDL1_Surface *surface, SDL1_Color *colors, int firstcolor, int ncolors) {
	return SDL_SetPalette(surface, SDL1_LOGPAL | SDL1_PHYSPAL, colors, firstcolor, ncolors);
}

static void convert_surface_dst (SDL1_Surface *src, SDL1_Surface *dst) {
	Uint32 colorkey = 0;
	Uint8 kr, kg, kb, alpha = 0;
	Uint32 srcflags;
	SDL1_Rect rect = { 0, 0, src->w, src->h };
	srcflags = src->flags;
	if (srcflags & SDL1_SRCCOLORKEY) {
		if (!(dst->flags & SDL1_SRCCOLORKEY) && dst->format->Amask) {
			srcflags &= ~SDL1_SRCCOLORKEY;
		} else {
			colorkey = src->format->colorkey;
			SDL_SetColorKey(src, 0, 0);
		}
	}
	if (srcflags & SDL1_SRCALPHA) {
		if (dst->format->Amask) {
			srcflags &= ~SDL1_SRCALPHA;
		} else {
			alpha = src->format->alpha;
			SDL_SetAlpha(src, 0, 0);
		}
	}
	SDL_LowerBlit(src, &rect, dst, &rect);
	SDL_SetClipRect(dst, &src->clip_rect);
	if (srcflags & SDL1_SRCCOLORKEY) {
		SDL_GetRGB(colorkey, src->format, &kr, &kg, &kb);
		SDL_SetColorKey(dst, srcflags & (SDL1_SRCCOLORKEY | SDL1_RLEACCEL), SDL_MapRGB(dst->format, kr, kg, kb));
		SDL_SetColorKey(src, srcflags & (SDL1_SRCCOLORKEY | SDL1_RLEACCEL), colorkey);
	}
	if (srcflags & SDL1_SRCALPHA) {
		SDL_SetAlpha(dst, srcflags & (SDL1_SRCALPHA | SDL1_RLEACCEL), alpha);
		SDL_SetAlpha(src, srcflags & (SDL1_SRCALPHA | SDL1_RLEACCEL), alpha);
	}
}

DECLSPEC SDL1_Surface *SDLCALL SDL_ConvertSurface (SDL1_Surface *src, SDL1_PixelFormat *fmt, Uint32 flags) {
	SDL1_Surface *dst;
	dst = SDL_CreateRGBSurface(flags, src->w, src->h, fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
	if (!dst) return NULL;
	if (fmt->palette) SDL_SetColors(dst, fmt->palette->colors, 0, fmt->palette->ncolors);
	convert_surface_dst(src, dst);
	return dst;
}

DECLSPEC SDL1_Surface *SDL_DisplayFormat (SDL1_Surface *surface) {
	if (!SDLCL_surface) return NULL;
	return SDL_ConvertSurface(surface, SDLCL_surface->format, 0);
}

DECLSPEC SDL1_Surface *SDL_DisplayFormatAlpha (SDL1_Surface *surface) {
	int depth;
	Uint32 Rmask, Gmask, Bmask, Amask;
	SDL1_Surface *dst;
	rSDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888, &depth, &Rmask, &Gmask, &Bmask, &Amask);
	dst = SDL_CreateRGBSurface(0, surface->w, surface->h, depth, Rmask, Gmask, Bmask, Amask);
	if (!dst) return NULL;
	convert_surface_dst(surface, dst);
	return dst;
}

DECLSPEC int SDLCALL SDL_Flip (SDL1_Surface *screen) {
	Uint8 *src;
	Uint32 *dest;
	void *texpix;
	int texpitch, i, j;
	(void)screen;
	if (!SDLCL_renderer) return 0;
	if (SDL_LockSurface(SDLCL_surface)) return -1;
	if (rSDL_LockTexture(main_texture, NULL, &texpix, &texpitch)) {
		SDL_UnlockSurface(SDLCL_surface);
		return -1;
	}
	if (SDLCL_surface->format->BitsPerPixel > 8) {
		for (i = 0; i < SDLCL_surface->h; i++)
			memcpy(((Uint8 *)texpix) + i * texpitch,
				SDLCL_surface->pixels + i * SDLCL_surface->pitch,
				SDLCL_surface->w * SDLCL_surface->format->BytesPerPixel);
	} else {
		for (i = 0; i < SDLCL_surface->h; i++) {
			src = SDLCL_surface->pixels + i * SDLCL_surface->pitch;
			dest = (Uint32 *)(((Uint8 *)texpix) + i * texpitch);
			for (j = 0; j < SDLCL_surface->w; j++)
				dest[j] = physical_palette[src[j]];
		}
	}
	/* TODO: Draw mouse cursor when scaling */
	rSDL_UnlockTexture(main_texture);
	SDL_UnlockSurface(SDLCL_surface);
	rSDL_RenderClear(SDLCL_renderer);
	if (SDLCL_scaling)
		rSDL_RenderCopy(SDLCL_renderer, main_texture, NULL, &SDLCL_scale_rect);
	else
		rSDL_RenderCopy(SDLCL_renderer, main_texture, NULL, NULL);
	rSDL_RenderPresent(SDLCL_renderer);
	return 0;
}

/* TODO: Implement partial screen updates? */
DECLSPEC void SDLCALL SDL_UpdateRect (SDL1_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h) {
	(void)x;
	(void)y;
	(void)w;
	(void)h;
	SDL_Flip(screen);
}

/* TODO: Implement partial screen updates? */
DECLSPEC void SDLCALL SDL_UpdateRects(SDL1_Surface *screen, int numrects, SDL1_Rect *rects)
{
	(void)numrects;
	(void)rects;
	SDL_Flip(screen);
}

DECLSPEC int SDLCALL SDL_GetGammaRamp (Uint16 *redtable, Uint16 *greentable, Uint16 *bluetable) {
	if (SDLCL_window) return rSDL_GetWindowGammaRamp(SDLCL_window, redtable, greentable, bluetable);
	else return -1;
}

DECLSPEC int SDLCALL SDL_SetGammaRamp (Uint16 *redtable, Uint16 *greentable, Uint16 *bluetable) {
	if (SDLCL_window) return rSDL_SetWindowGammaRamp(SDLCL_window, redtable, greentable, bluetable);
	else return -1;
}

DECLSPEC int SDLCALL SDL_SetGamma (float redgamma, float greengamma, float bluegamma) {
	Uint16 redtable[256], greentable[256], bluetable[256];
	rSDL_CalculateGammaRamp(redgamma, redtable);
	rSDL_CalculateGammaRamp(greengamma, greentable);
	rSDL_CalculateGammaRamp(bluegamma, bluetable);
	return SDL_SetGammaRamp(redtable, greentable, bluetable);
}

DECLSPEC int SDLCALL SDL_GL_LoadLibrary (const char *path) {
	int ret = rSDL_GL_LoadLibrary(path);
	if (ret) printf("Error loading GL: %s", rSDL_GetError());
	return ret;
}

DECLSPEC void *SDLCALL SDL_GL_GetProcAddress (const char *proc) {
	return rSDL_GL_GetProcAddress(proc);
}

DECLSPEC void SDLCALL SDL_GL_SwapBuffers (void) {
	if (SDLCL_scaling) gl_scale();
	rSDL_GL_SwapWindow(SDLCL_window);
}

typedef enum {
	SDL1_GL_RED_SIZE,
	SDL1_GL_GREEN_SIZE,
	SDL1_GL_BLUE_SIZE,
	SDL1_GL_ALPHA_SIZE,
	SDL1_GL_BUFFER_SIZE,
	SDL1_GL_DOUBLEBUFFER,
	SDL1_GL_DEPTH_SIZE,
	SDL1_GL_STENCIL_SIZE,
	SDL1_GL_ACCUM_RED_SIZE,
	SDL1_GL_ACCUM_GREEN_SIZE,
	SDL1_GL_ACCUM_BLUE_SIZE,
	SDL1_GL_ACCUM_ALPHA_SIZE,
	SDL1_GL_STEREO,
	SDL1_GL_MULTISAMPLEBUFFERS,
	SDL1_GL_MULTISAMPLESAMPLES,
	SDL1_GL_ACCELERATED_VISUAL,
	SDL1_GL_SWAP_CONTROL
} SDL1_GLattr;

static int glattr1to2 (SDL1_GLattr attr, SDL_GLattr *out) {
	switch (attr) {
		case SDL1_GL_RED_SIZE:
			*out = SDL_GL_RED_SIZE;
			return 1;
		case SDL1_GL_GREEN_SIZE:
			*out = SDL_GL_GREEN_SIZE;
			return 1;
		case SDL1_GL_BLUE_SIZE:
			*out = SDL_GL_BLUE_SIZE;
			return 1;
		case SDL1_GL_ALPHA_SIZE:
			*out = SDL_GL_ALPHA_SIZE;
			return 1;
		case SDL1_GL_BUFFER_SIZE:
			*out = SDL_GL_BUFFER_SIZE;
			return 1;
		case SDL1_GL_DOUBLEBUFFER:
			*out = SDL_GL_DOUBLEBUFFER;
			return 1;
		case SDL1_GL_DEPTH_SIZE:
			*out = SDL_GL_DEPTH_SIZE;
			return 1;
		case SDL1_GL_STENCIL_SIZE:
			*out = SDL_GL_STENCIL_SIZE;
			return 1;
		case SDL1_GL_ACCUM_RED_SIZE:
			*out = SDL_GL_ACCUM_RED_SIZE;
			return 1;
		case SDL1_GL_ACCUM_GREEN_SIZE:
			*out = SDL_GL_ACCUM_GREEN_SIZE;
			return 1;
		case SDL1_GL_ACCUM_BLUE_SIZE:
			*out = SDL_GL_ACCUM_BLUE_SIZE;
			return 1;
		case SDL1_GL_ACCUM_ALPHA_SIZE:
			*out = SDL_GL_ACCUM_ALPHA_SIZE;
			return 1;
		case SDL1_GL_STEREO:
			*out = SDL_GL_STEREO;
			return 1;
		case SDL1_GL_MULTISAMPLEBUFFERS:
			*out = SDL_GL_MULTISAMPLEBUFFERS;
			return 1;
		case SDL1_GL_MULTISAMPLESAMPLES:
			*out = SDL_GL_MULTISAMPLESAMPLES;
			return 1;
		case SDL1_GL_ACCELERATED_VISUAL:
			*out = SDL_GL_ACCELERATED_VISUAL;
			return 1;
		default:
			return 0;
	}
}

static int swap_control = 1;

DECLSPEC int SDLCALL SDL_GL_SetAttribute (SDL1_GLattr attr, int value) {
	SDL_GLattr attr2;
	if (glattr1to2(attr, &attr2)) {
		return rSDL_GL_SetAttribute(attr2, value);
	} else {
		switch (attr) {
			case SDL1_GL_SWAP_CONTROL:
				swap_control = value;
				return 0;
			default:
				return -1;
		}
	}
}

DECLSPEC int SDLCALL SDL_GL_GetAttribute (SDL1_GLattr attr, int *value) {
	SDL_GLattr attr2;
	if (glattr1to2(attr, &attr2)) {
		return rSDL_GL_GetAttribute(attr2, value);
	} else {
		switch (attr) {
			case SDL1_GL_SWAP_CONTROL:
				*value = swap_control;
				return 0;
			default:
				return -1;
		}
	}
}

DECLSPEC void SDLCALL SDL_GL_DisableContext (void) {
	rSDL_GL_MakeCurrent(SDLCL_window, NULL);
}

DECLSPEC void SDLCALL SDL_GL_EnableContext_Thread (void) {
	rSDL_GL_MakeCurrent(SDLCL_window, main_glcontext);
}

DECLSPEC void SDLCALL SDL_WM_SetCaption (const char *title, const char *icon) {
	char *newstr;
	if (title) {
		newstr = malloc(strlen(title) + 1);
		strcpy(newstr, title);
		free(window_title);
		window_title = newstr;
	}
	/* Although SDL2 doesn't impleemnt icon titles, save it for SDL_WM_GetCaption() */
	if (icon) {
		newstr = malloc(strlen(icon) + 1);
		strcpy(newstr, icon);
		free(window_icon);
		window_icon = newstr;
	}
	if (title && SDLCL_window) rSDL_SetWindowTitle(SDLCL_window, window_title);
}

DECLSPEC void SDL_WM_GetCaption(char **title, char **icon) {
	if (title) *title = window_title;
	if (icon) *icon = window_icon;
}

DECLSPEC void SDLCALL SDL_WM_SetIcon (SDL_Surface *icon, Uint8 *mask) {
	(void)icon;
	(void)mask;
}

typedef enum {
	SDL1_GRAB_QUERY = -1,
	SDL1_GRAB_OFF = 0,
	SDL1_GRAB_ON = 1,
	SDL1_GRAB_FULLSCREEN
} SDL1_GrabMode;

DECLSPEC SDL1_GrabMode SDLCALL SDL_WM_GrabInput (SDL1_GrabMode mode) {
	SDL1_GrabMode ret = grab ? SDL1_GRAB_ON : SDL1_GRAB_OFF;
	switch (mode) {
		case SDL1_GRAB_OFF: grab = SDL_FALSE; break;
		case SDL1_GRAB_ON: grab = SDL_TRUE; break;
		default: break;
	}
	SDLCL_UpdateGrab();
	return ret;
}

DECLSPEC int SDLCALL SDL_WM_IconifyWindow (void) {
	rSDL_MinimizeWindow(SDLCL_window);
	return 1;
}

DECLSPEC int SDLCALL SDL_WM_ToggleFullScreen (SDL1_Surface *surface) {
	(void)surface;
	return 0;
}

typedef enum {
	SDL1_SYSWM_X11
} SDL1_SYSWM_TYPE;

typedef struct SDL1_SysWMinfo {
	SDL1_version version;
	SDL1_SYSWM_TYPE subsystem;
	union {
#if defined(SDL_VIDEO_DRIVER_X11)
		struct {
			Display *display;
			Window window;
			void (*lock_func)(void);
			void (*unlock_func)(void);
			/* SDL 1.0.2 */
			Window fswindow;
			Window wmwindow;
			/* SDL 1.2.12 */
			Display *gfxdisplay;
		} x11;
#endif
		int dummy;
	} info;
} SDL1_SysWMinfo;

static int compare_ver (SDL1_version version, int major, int minor, int patch) {
	if (version.major == major) {
		if (version.minor == minor) {
			return version.patch >= patch;
		} else {
			return version.minor >= minor;
		}
	} else {
		return version.major >= major;
	}
}

static void noop (void) {
}

DECLSPEC int SDLCALL SDL_GetWMInfo (SDL1_SysWMinfo *info) {
	SDL_SysWMinfo info2;
	if (!SDLCL_window) return 0;
	SDL_VERSION(&info2.version);
	if (rSDL_GetWindowWMInfo(SDLCL_window, &info2)) {
#if defined(SDL_VIDEO_DRIVER_X11)
		if (info2.subsystem == SDL_SYSWM_X11 && compare_ver(info->version, 1, 0, 0)) {
			info->subsystem = SDL1_SYSWM_X11;

			info->info.x11.display = info2.info.x11.display;
			info->info.x11.window = info2.info.x11.window;
			info->info.x11.lock_func = noop;
			info->info.x11.unlock_func = noop;
			if (compare_ver(info->version, 1, 0, 2)) {
				info->info.x11.fswindow = 0;
				info->info.x11.wmwindow = 0;
			}
			if (compare_ver(info->version, 1, 2, 12))
				info->info.x11.gfxdisplay = info2.info.x11.display;
			return 1;
		}
#endif
	}
	return 0;
}

DECLSPEC int SDL_SoftStretch (SDL1_Surface *src, SDL1_Rect *srcrect, SDL1_Surface *dst, SDL1_Rect *dstrect) {
	SDL_Rect srcrect2, dstrect2;
	SDL_Rect *srcptr = NULL;
	SDL_Rect *dstptr = NULL;
	if (srcrect) {
		srcrect2.x = srcrect->x;
		srcrect2.y = srcrect->y;
		srcrect2.w = srcrect->w;
		srcrect2.h = srcrect->h;
		srcptr = &srcrect2;
	}
	if (dstrect) {
		dstrect2.x = dstrect->x;
		dstrect2.y = dstrect->y;
		dstrect2.w = dstrect->w;
		dstrect2.h = dstrect->h;
		dstptr = &dstrect2;
	}
	return rSDL_SoftStretch(src->sdl2_surface, srcptr, dst->sdl2_surface, dstptr);
}

#if defined(SDL_VIDEO_DRIVER_X11)
#include <X11/Xutil.h>
#include <X11/keysym.h>

static SDL1Key ODD_keymap[256];
static SDL1Key MISC_keymap[256];

static void *x11_lib = NULL;
static KeyCode (*rXKeysymToKeycode) (Display *a, KeySym b) = NULL;
#if NeedWidePrototypes
static KeySym (*rXKeycodeToKeysym) (Display *a, unsigned int b, int c) = NULL;
#else
static KeySym (*rXKeycodeToKeysym) (Display *a, KeyCode b, int c) = NULL;
#endif
static int (*rXLookupString) (XKeyEvent *a, char *b, int c, KeySym *d, XComposeStatus *e) = NULL;
static XModifierKeymap *(*rXGetModifierMapping) (Display *a) = NULL;
static int (*rXFreeModifiermap) (XModifierKeymap *a) = NULL;

static int init_x11_data (void) {
	static int init = 0;
	int i;
	if (init) return 1;

	x11_lib = SDL_LoadObject("libX11.so.6");
	if (!x11_lib) return 0;
	rXKeysymToKeycode = SDL_LoadFunction(x11_lib, "XKeysymToKeycode");
	rXKeycodeToKeysym = SDL_LoadFunction(x11_lib, "XKeycodeToKeysym");
	rXLookupString = SDL_LoadFunction(x11_lib, "XLookupString");
	rXGetModifierMapping = SDL_LoadFunction(x11_lib, "XGetModifierMapping");
	rXFreeModifiermap = SDL_LoadFunction(x11_lib, "XFreeModifiermap");
	if (!rXKeysymToKeycode || !rXKeycodeToKeysym || !rXLookupString || !rXGetModifierMapping || !rXFreeModifiermap) {
		SDL_UnloadObject(x11_lib);
		return 0;
	}

	/* Odd keys used in international keyboards */
	for (i = 0; i < 256; i++) ODD_keymap[i] = SDLK1_UNKNOWN;
	/* These X keysyms have 0xFE as the high byte */
	ODD_keymap[XK_dead_grave&0xFF] = SDLK1_COMPOSE;
	ODD_keymap[XK_dead_acute&0xFF] = SDLK1_COMPOSE;
	ODD_keymap[XK_dead_tilde&0xFF] = SDLK1_COMPOSE;
	ODD_keymap[XK_dead_macron&0xFF] = SDLK1_COMPOSE;
	ODD_keymap[XK_dead_breve&0xFF] = SDLK1_COMPOSE;
	ODD_keymap[XK_dead_abovedot&0xFF] = SDLK1_COMPOSE;
	ODD_keymap[XK_dead_diaeresis&0xFF] = SDLK1_COMPOSE;
	ODD_keymap[XK_dead_abovering&0xFF] = SDLK1_COMPOSE;
	ODD_keymap[XK_dead_doubleacute&0xFF] = SDLK1_COMPOSE;
	ODD_keymap[XK_dead_caron&0xFF] = SDLK1_COMPOSE;
	ODD_keymap[XK_dead_cedilla&0xFF] = SDLK1_COMPOSE;
	ODD_keymap[XK_dead_ogonek&0xFF] = SDLK1_COMPOSE;
	ODD_keymap[XK_dead_iota&0xFF] = SDLK1_COMPOSE;
	ODD_keymap[XK_dead_voiced_sound&0xFF] = SDLK1_COMPOSE;
	ODD_keymap[XK_dead_semivoiced_sound&0xFF] = SDLK1_COMPOSE;
	ODD_keymap[XK_dead_belowdot&0xFF] = SDLK1_COMPOSE;
#ifdef XK_dead_hook
	ODD_keymap[XK_dead_hook&0xFF] = SDLK1_COMPOSE;
#endif
#ifdef XK_dead_horn
	ODD_keymap[XK_dead_horn&0xFF] = SDLK1_COMPOSE;
#endif

#ifdef XK_dead_circumflex
	ODD_keymap[XK_dead_circumflex&0xFF] = SDLK1_CARET;
#endif
#ifdef XK_ISO_Level3_Shift
	ODD_keymap[XK_ISO_Level3_Shift&0xFF] = SDLK1_MODE; /* "Alt Gr" key */
#endif

	/* Map the miscellaneous keys */
	for (i = 0; i < 256; i++) MISC_keymap[i] = SDLK1_UNKNOWN;
	/* These X keysyms have 0xFF as the high byte */
	MISC_keymap[XK_BackSpace&0xFF] = SDLK1_BACKSPACE;
	MISC_keymap[XK_Tab&0xFF] = SDLK1_TAB;
	MISC_keymap[XK_Clear&0xFF] = SDLK1_CLEAR;
	MISC_keymap[XK_Return&0xFF] = SDLK1_RETURN;
	MISC_keymap[XK_Pause&0xFF] = SDLK1_PAUSE;
	MISC_keymap[XK_Escape&0xFF] = SDLK1_ESCAPE;
	MISC_keymap[XK_Delete&0xFF] = SDLK1_DELETE;

	MISC_keymap[XK_KP_0&0xFF] = SDLK1_KP0; /* Keypad 0-9 */
	MISC_keymap[XK_KP_1&0xFF] = SDLK1_KP1;
	MISC_keymap[XK_KP_2&0xFF] = SDLK1_KP2;
	MISC_keymap[XK_KP_3&0xFF] = SDLK1_KP3;
	MISC_keymap[XK_KP_4&0xFF] = SDLK1_KP4;
	MISC_keymap[XK_KP_5&0xFF] = SDLK1_KP5;
	MISC_keymap[XK_KP_6&0xFF] = SDLK1_KP6;
	MISC_keymap[XK_KP_7&0xFF] = SDLK1_KP7;
	MISC_keymap[XK_KP_8&0xFF] = SDLK1_KP8;
	MISC_keymap[XK_KP_9&0xFF] = SDLK1_KP9;
	MISC_keymap[XK_KP_Insert&0xFF] = SDLK1_KP0;
	MISC_keymap[XK_KP_End&0xFF] = SDLK1_KP1;
	MISC_keymap[XK_KP_Down&0xFF] = SDLK1_KP2;
	MISC_keymap[XK_KP_Page_Down&0xFF] = SDLK1_KP3;
	MISC_keymap[XK_KP_Left&0xFF] = SDLK1_KP4;
	MISC_keymap[XK_KP_Begin&0xFF] = SDLK1_KP5;
	MISC_keymap[XK_KP_Right&0xFF] = SDLK1_KP6;
	MISC_keymap[XK_KP_Home&0xFF] = SDLK1_KP7;
	MISC_keymap[XK_KP_Up&0xFF] = SDLK1_KP8;
	MISC_keymap[XK_KP_Page_Up&0xFF] = SDLK1_KP9;
	MISC_keymap[XK_KP_Delete&0xFF] = SDLK1_KP_PERIOD;
	MISC_keymap[XK_KP_Decimal&0xFF] = SDLK1_KP_PERIOD;
	MISC_keymap[XK_KP_Divide&0xFF] = SDLK1_KP_DIVIDE;
	MISC_keymap[XK_KP_Multiply&0xFF] = SDLK1_KP_MULTIPLY;
	MISC_keymap[XK_KP_Subtract&0xFF] = SDLK1_KP_MINUS;
	MISC_keymap[XK_KP_Add&0xFF] = SDLK1_KP_PLUS;
	MISC_keymap[XK_KP_Enter&0xFF] = SDLK1_KP_ENTER;
	MISC_keymap[XK_KP_Equal&0xFF] = SDLK1_KP_EQUALS;

	MISC_keymap[XK_Up&0xFF] = SDLK1_UP;
	MISC_keymap[XK_Down&0xFF] = SDLK1_DOWN;
	MISC_keymap[XK_Right&0xFF] = SDLK1_RIGHT;
	MISC_keymap[XK_Left&0xFF] = SDLK1_LEFT;
	MISC_keymap[XK_Insert&0xFF] = SDLK1_INSERT;
	MISC_keymap[XK_Home&0xFF] = SDLK1_HOME;
	MISC_keymap[XK_End&0xFF] = SDLK1_END;
	MISC_keymap[XK_Page_Up&0xFF] = SDLK1_PAGEUP;
	MISC_keymap[XK_Page_Down&0xFF] = SDLK1_PAGEDOWN;

	MISC_keymap[XK_F1&0xFF] = SDLK1_F1;
	MISC_keymap[XK_F2&0xFF] = SDLK1_F2;
	MISC_keymap[XK_F3&0xFF] = SDLK1_F3;
	MISC_keymap[XK_F4&0xFF] = SDLK1_F4;
	MISC_keymap[XK_F5&0xFF] = SDLK1_F5;
	MISC_keymap[XK_F6&0xFF] = SDLK1_F6;
	MISC_keymap[XK_F7&0xFF] = SDLK1_F7;
	MISC_keymap[XK_F8&0xFF] = SDLK1_F8;
	MISC_keymap[XK_F9&0xFF] = SDLK1_F9;
	MISC_keymap[XK_F10&0xFF] = SDLK1_F10;
	MISC_keymap[XK_F11&0xFF] = SDLK1_F11;
	MISC_keymap[XK_F12&0xFF] = SDLK1_F12;
	MISC_keymap[XK_F13&0xFF] = SDLK1_F13;
	MISC_keymap[XK_F14&0xFF] = SDLK1_F14;
	MISC_keymap[XK_F15&0xFF] = SDLK1_F15;

	MISC_keymap[XK_Num_Lock&0xFF] = SDLK1_NUMLOCK;
	MISC_keymap[XK_Caps_Lock&0xFF] = SDLK1_CAPSLOCK;
	MISC_keymap[XK_Scroll_Lock&0xFF] = SDLK1_SCROLLOCK;
	MISC_keymap[XK_Shift_R&0xFF] = SDLK1_RSHIFT;
	MISC_keymap[XK_Shift_L&0xFF] = SDLK1_LSHIFT;
	MISC_keymap[XK_Control_R&0xFF] = SDLK1_RCTRL;
	MISC_keymap[XK_Control_L&0xFF] = SDLK1_LCTRL;
	MISC_keymap[XK_Alt_R&0xFF] = SDLK1_RALT;
	MISC_keymap[XK_Alt_L&0xFF] = SDLK1_LALT;
	MISC_keymap[XK_Meta_R&0xFF] = SDLK1_RMETA;
	MISC_keymap[XK_Meta_L&0xFF] = SDLK1_LMETA;
	MISC_keymap[XK_Super_L&0xFF] = SDLK1_LSUPER; /* Left "Windows" */
	MISC_keymap[XK_Super_R&0xFF] = SDLK1_RSUPER; /* Right "Windows */
	MISC_keymap[XK_Mode_switch&0xFF] = SDLK1_MODE; /* "Alt Gr" key */
	MISC_keymap[XK_Multi_key&0xFF] = SDLK1_COMPOSE; /* Multi-key compose */

	MISC_keymap[XK_Help&0xFF] = SDLK1_HELP;
	MISC_keymap[XK_Print&0xFF] = SDLK1_PRINT;
	MISC_keymap[XK_Sys_Req&0xFF] = SDLK1_SYSREQ;
	MISC_keymap[XK_Break&0xFF] = SDLK1_BREAK;
	MISC_keymap[XK_Menu&0xFF] = SDLK1_MENU;
	MISC_keymap[XK_Hyper_R&0xFF] = SDLK1_MENU; /* Windows "Menu" key */

	init = 1;
	return 1;
}

/* X11 modifier masks for various keys */
static unsigned int meta_l_mask, meta_r_mask, alt_l_mask, alt_r_mask;
static unsigned int num_mask, mode_switch_mask;

static void get_modifier_masks (Display *display) {
	static int got_masks = 0;
	int i, j, n;
	XModifierKeymap *xmods;
	if (got_masks) return;
	xmods = rXGetModifierMapping(display);
	n = xmods->max_keypermod;
	for (i = 3; i < 8; i++) {
		for (j = 0; j < n; j++) {
			KeyCode kc = xmods->modifiermap[i * n + j];
			KeySym ks = rXKeycodeToKeysym(display, kc, 0);
			unsigned int mask = 1 << i;
			switch (ks) {
				case XK_Num_Lock: num_mask = mask; break;
				case XK_Alt_L: alt_l_mask = mask; break;
				case XK_Alt_R: alt_r_mask = mask; break;
				case XK_Meta_L: meta_l_mask = mask; break;
				case XK_Meta_R: meta_r_mask = mask; break;
				case XK_Mode_switch: mode_switch_mask = mask; break;
			}
		}
	}
	rXFreeModifiermap(xmods);
	got_masks = 1;
}

DECLSPEC Uint16 SDLCALL X11_KeyToUnicode (SDL1Key keysym, SDL1Mod modifiers) {
	Display *display;
	SDL1_SysWMinfo info;
	char keybuf[32];
	int i;
	KeySym xsym = 0;
	XKeyEvent xkey;
	Uint16 unicode;

	if (!init_x11_data()) return 0;
	info.version.major = SDL1_MAJOR_VERSION;
	info.version.minor = SDL1_MINOR_VERSION;
	info.version.patch = SDL1_PATCHLEVEL;
	if (!SDL_GetWMInfo(&info)) return 0;
	if (info.subsystem != SDL1_SYSWM_X11) return 0;
	display = info.info.x11.display;

	memset(&xkey, 0, sizeof(xkey));
	xkey.display = display;

	xsym = keysym; /* last resort if not found */
	for (i = 0; i < 256; i++) {
		if (MISC_keymap[i] == keysym) {
			xsym = 0xFF00 | i;
			break;
		} else if (ODD_keymap[i] == keysym) {
			xsym = 0xFE00 | i;
			break;
		}
	}

	xkey.keycode = rXKeysymToKeycode(xkey.display, xsym);

	get_modifier_masks(display);
	if (modifiers & KMOD1_SHIFT) xkey.state |= ShiftMask;
	if (modifiers & KMOD1_CAPS) xkey.state |= LockMask;
	if (modifiers & KMOD1_CTRL) xkey.state |= ControlMask;
	if (modifiers & KMOD1_MODE) xkey.state |= mode_switch_mask;
	if (modifiers & KMOD1_LALT) xkey.state |= alt_l_mask;
	if (modifiers & KMOD1_RALT) xkey.state |= alt_r_mask;
	if (modifiers & KMOD1_LMETA) xkey.state |= meta_l_mask;
	if (modifiers & KMOD1_RMETA) xkey.state |= meta_r_mask;
	if (modifiers & KMOD1_NUM) xkey.state |= num_mask;

	unicode = 0;
	if (rXLookupString(&xkey, keybuf, sizeof(keybuf), NULL, NULL))
		unicode = (unsigned char)keybuf[0];
	return unicode;
}
#else
DECLSPEC Uint16 SDLCALL X11_KeyToUnicode (SDL1Key keysym, SDL1Mod modifiers) {
	(void)keysym;
	(void)modifiers;
	return 0;
}
#endif

/* These functions are used internally for the implementation of SDL_OPENGLBLIT. */
/* Since we don't implement it, just do nothing here. */
DECLSPEC void SDLCALL SDL_GL_UpdateRects (int numrects, SDL_Rect *rects) {
	(void)numrects;
	(void)rects;
}

DECLSPEC void SDLCALL SDL_GL_Lock (void) {
}

DECLSPEC void SDLCALL SDL_GL_Unlock (void) {
}
