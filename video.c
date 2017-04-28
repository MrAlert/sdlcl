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
#include <stdio.h>

#include "SDL2.h"
#include "rwops.h"

typedef struct SDL1_Rect {
	Sint16 x, y;
	Uint16 w, h;
} SDL1_Rect;

typedef struct SDL1_Color {
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 unused;
} SDL1_Color;

typedef struct SDL1_Palette {
	int ncolors;
	SDL1_Color *colors;
} SDL1_Palette;

typedef struct SDL1_PixelFormat {
	SDL1_Palette *palette;
	Uint8 BitsPerPixel;
	Uint8 BytesPerPixel;
	Uint8 Rloss;
	Uint8 Gloss;
	Uint8 Bloss;
	Uint8 Aloss;
	Uint8 Rshift;
	Uint8 Gshift;
	Uint8 Bshift;
	Uint8 Ashift;
	Uint32 Rmask;
	Uint32 Gmask;
	Uint32 Bmask;
	Uint32 Amask;
	Uint32 colorkey;
	Uint8 alpha;
} SDL1_PixelFormat;

typedef struct SDL1_Surface {
	Uint32 flags;
	SDL1_PixelFormat *format;
	int w, h;
	Uint16 pitch;
	void *pixels;
	int offset;
	SDL_Surface *sdl2_surface;
	SDL1_Rect clip_rect;
	Uint32 unused1;
	Uint32 private3;
	void *private4;
	unsigned int private5;
	int refcount;
} SDL1_Surface;


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

static SDL1_Surface *SDLCL_CreateSurfaceFromSDL2(SDL_Surface *surface2) {
	SDL1_Proxy *proxy;
	int i, ncolors = surface2->format->palette ? surface2->format->palette->ncolors : 0;
	SDL_Color *color;
	proxy = malloc(sizeof(SDL1_Proxy) + ncolors * sizeof(SDL1_Color));
	if (!proxy) {
		return NULL;
	}
	proxy->surface.flags = SDL_MUSTLOCK(surface2) ? SDL_RLEACCEL : 0;
	proxy->surface.format = &proxy->format;
	proxy->surface.w = surface2->w;
	proxy->surface.h = surface2->h;
	proxy->surface.pitch = surface2->pitch;
	proxy->surface.pixels = surface2->pixels;
	proxy->surface.offset = 0;
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
	proxy->format.Rloss = masktoloss(proxy->format.Rmask);
	proxy->format.Gloss = masktoloss(proxy->format.Gmask);
	proxy->format.Bloss = masktoloss(proxy->format.Bmask);
	proxy->format.Aloss = masktoloss(proxy->format.Amask);
	proxy->format.Rshift = masktoshift(proxy->format.Rmask);
	proxy->format.Gshift = masktoshift(proxy->format.Gmask);
	proxy->format.Bshift = masktoshift(proxy->format.Bmask);
	proxy->format.Ashift = masktoshift(proxy->format.Amask);
	proxy->format.colorkey = 0;
	proxy->format.alpha = 255;
	return &proxy->surface;
}

SDL1_Surface *SDLCALL SDL_CreateRGBSurface (Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
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
	if (surface->flags & SDL1_SRCALPHA) rSDL_SetSurfaceBlendMode(surface2, SDL_BLENDMODE_BLEND);
	else rSDL_SetSurfaceBlendMode(surface2, SDL_BLENDMODE_NONE);
	return surface;
}

SDL1_Surface *SDLCALL SDL_CreateRGBSurfaceFrom (void *pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
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

int SDLCALL SDL_FillRect (SDL1_Surface *dst, SDL1_Rect *dstrect, Uint32 color) {
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
int SDLCALL SDL_UpperBlit (SDL1_Surface *src, SDL1_Rect *srcrect, SDL1_Surface *dst, SDL1_Rect *dstrect) {
	SDL_Rect srcrect2, *srcptr = NULL;
	SDL_Rect dstrect2, *dstptr = NULL;
	int ret;
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

int SDLCALL SDL_LockSurface (SDL1_Surface *surface) {
	int ret = rSDL_LockSurface(surface->sdl2_surface);
	surface->pixels = surface->sdl2_surface->pixels;
	return ret;
}

void SDLCALL SDL_UnlockSurface (SDL1_Surface *surface) {
	rSDL_UnlockSurface(surface->sdl2_surface);
}

void SDLCALL SDL_FreeSurface (SDL1_Surface *surface) {
	SDL1_Proxy *proxy = (SDL1_Proxy *)surface;
	rSDL_FreeSurface(surface->sdl2_surface);
	free(proxy);
}

SDL1_Surface *SDLCALL SDL_LoadBMP_RW(SDL1_RWops *src, int freesrc) {
	SDL1_Surface *surface;
	SDL_Surface *surface2;
	SDL_RWops *src2 = SDLCL_RWFromSDL1(src);
	if (!src2) {
		if (freesrc && src) SDL1_RWclose(src);
		return NULL;
	}
	surface2 = rSDL_LoadBMP_RW(src2, freesrc);
	if (!freesrc) rSDL_FreeRW(src2);
	surface = SDLCL_CreateSurfaceFromSDL2(surface2);
	if (!surface) {
		rSDL_FreeSurface(surface2);
		return NULL;
	}
	return surface;
}

static Uint32 map_component (Uint8 v, Uint8 loss, Uint8 shift, Uint32 mask) {
	return (((Uint32)v >> loss) << shift) & mask;
}

Uint32 SDLCALL SDL_MapRGBA (SDL1_PixelFormat *fmt, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	Uint32 color;
	/* TODO: Map palette formats */
	if (fmt->palette) return 0;
	color = map_component(r, fmt->Rloss, fmt->Rshift, fmt->Rmask);
	color |= map_component(g, fmt->Gloss, fmt->Gshift, fmt->Gmask);
	color |= map_component(b, fmt->Bloss, fmt->Bshift, fmt->Bmask);
	color |= map_component(a, fmt->Aloss, fmt->Ashift, fmt->Amask);
	return color;
}

Uint32 SDLCALL SDL_MapRGB (SDL1_PixelFormat *fmt, Uint8 r, Uint8 g, Uint8 b) {
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

void SDLCALL SDL_GetRGBA (Uint32 pixel, SDL1_PixelFormat *fmt, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a) {
	*r = get_component(pixel, fmt->Rloss, fmt->Rshift, fmt->Rmask);
	*g = get_component(pixel, fmt->Gloss, fmt->Gshift, fmt->Gmask);
	*b = get_component(pixel, fmt->Bloss, fmt->Bshift, fmt->Bmask);
	if (fmt->Amask) *a = get_component(pixel, fmt->Aloss, fmt->Ashift, fmt->Amask);
	else *a = 255;
}

void SDLCALL SDL_GetRGB (Uint32 pixel, SDL1_PixelFormat *fmt, Uint8 *r, Uint8 *g, Uint8 *b) {
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

SDL1_VideoInfo *SDLCALL SDL_GetVideoInfo (void) {
	return &video_info;
}

#define SDL_WINDOW_INVALID ((Uint32)-1)

static Uint32 vidflags1to2 (Uint32 flags) {
	Uint32 flags2 = 0;
	/* No OPENGLBLIT support */
	if (flags & SDL1_OPENGLBLIT & ~SDL1_OPENGL) return SDL_WINDOW_INVALID;
	if (flags & SDL1_FULLSCREEN) flags2 |= SDL_WINDOW_FULLSCREEN;
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

SDL1_Rect **SDLCALL SDL_ListModes (SDL1_PixelFormat *format, Uint32 flags) {
	(void)format;
	int i, nummode = rSDL_GetNumDisplayModes(0);
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
	return mode_list;
}

int SDLCALL SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags) {
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

static SDL_Window *main_window = NULL;
static SDL_Renderer *main_renderer = NULL;
static SDL_Texture *main_texture = NULL;
static SDL_GLContext main_glcontext = NULL;
static SDL1_Surface *main_surface = NULL;

static Uint32 mode_flags = 0;
static SDL_bool grab = SDL_FALSE;

static void close_window (void) {
	if (main_surface) {
		SDL_FreeSurface(main_surface);
		main_surface = NULL;
	}
	if (main_glcontext) {
		rSDL_GL_DeleteContext(main_glcontext);
		main_glcontext = NULL;
	}
	if (main_texture) {
		rSDL_DestroyTexture(main_texture);
		main_texture = NULL;
	}
	if (main_renderer) {
		rSDL_DestroyRenderer(main_renderer);
		main_renderer = NULL;
	}
	if (main_window) {
		rSDL_DestroyWindow(main_window);
		main_window = NULL;
	}
}

static void update_grab (void) {
	if (main_window)
		rSDL_SetWindowGrab(main_window, (mode_flags & SDL1_FULLSCREEN) ? SDL_TRUE : grab);
	if (/*grab &&*/ rSDL_ShowCursor(SDL_QUERY) == SDL_DISABLE)
		rSDL_SetRelativeMouseMode(SDL_TRUE);
	else
		rSDL_SetRelativeMouseMode(SDL_FALSE);
}

SDL1_Surface *SDLCALL SDL_SetVideoMode (int width, int height, int bpp, Uint32 flags) {
	Uint32 Rmask, Gmask, Bmask, Amask;
	Uint32 pixfmt = SDL_PIXELFORMAT_RGBX8888;
	Uint32 flags2 = vidflags1to2(flags);
	(void)width;
	(void)height;
	(void)bpp;
	if (flags2 == SDL_WINDOW_INVALID) return NULL;
	if (bpp && !(flags & SDL1_ANYFORMAT)) {
		switch (bpp) {
			case 8: pixfmt = SDL_PIXELFORMAT_INDEX8; break;
			case 15: pixfmt = SDL_PIXELFORMAT_RGB555; break;
			case 16: pixfmt = SDL_PIXELFORMAT_RGB565; break;
			case 24: pixfmt = SDL_PIXELFORMAT_RGB888; break;
			case 32: pixfmt = SDL_PIXELFORMAT_RGBX8888; break;
			default: return NULL;
		}
	}
	rSDL_PixelFormatEnumToMasks(pixfmt, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
	close_window();
	if (flags & SDL1_OPENGL) {
		/* Use compatibility profile for OpenGL */
		rSDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
		rSDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
		rSDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	}
	main_window = rSDL_CreateWindow("LOL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags2);
	if (!main_window) return NULL;
	if (flags & SDL1_OPENGL) {
		main_glcontext = rSDL_GL_CreateContext(main_window);
		if (!main_glcontext) {
			close_window();
			return NULL;
		}
	} else {
		main_renderer = rSDL_CreateRenderer(main_window, -1, 0);
		if (!main_renderer) {
			close_window();
			return NULL;
		}
		rSDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);
		main_texture = rSDL_CreateTexture(main_renderer, pixfmt, SDL_TEXTUREACCESS_STREAMING, width, height);
		if (!main_texture) {
			close_window();
			return NULL;
		}
		rSDL_SetTextureBlendMode(main_texture, SDL_BLENDMODE_NONE);
	}
	main_surface = SDL_CreateRGBSurface(0, width, height, bpp, Rmask, Gmask, Bmask, Amask);
	if (!main_surface) {
		close_window();
		return NULL;
	}
	if (flags & SDL1_FULLSCREEN) rSDL_SetWindowGrab(main_window, SDL_TRUE);
	mode_flags = flags;
	update_grab();
	return main_surface;
}

SDL1_Surface *SDLCALL SDL_GetVideoSurface (void) {
	return main_surface;
}

int SDLCALL SDL_Flip (SDL1_Surface *screen) {
	void *texpix;
	int texpitch, i;
	(void)screen;
	if (!main_renderer) return 0;
	if (SDL_LockSurface(main_surface)) return -1;
	if (rSDL_LockTexture(main_texture, NULL, &texpix, &texpitch)) {
		SDL_UnlockSurface(main_surface);
		return -1;
	}
	for (i = 0; i < main_surface->h; i++)
		memcpy(((Uint8 *)texpix) + i * texpitch, main_surface->pixels + i * main_surface->pitch, main_surface->w * main_surface->format->BytesPerPixel);
	rSDL_UnlockTexture(main_texture);
	SDL_UnlockSurface(main_surface);
	rSDL_RenderClear(main_renderer);
	rSDL_RenderCopy(main_renderer, main_texture, NULL, NULL);
	rSDL_RenderPresent(main_renderer);
	return 0;
}

/* TODO: Implement partial screen updates? */
void SDL_UpdateRect (SDL1_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h) {
	(void)x;
	(void)y;
	(void)w;
	(void)h;
	SDL_Flip(screen);
}

/* TODO: Implement partial screen updates? */
void SDLCALL SDL_UpdateRects(SDL1_Surface *screen, int numrects, SDL1_Rect *rects)
{
	(void)numrects;
	(void)rects;
	SDL_Flip(screen);
}

int SDLCALL SDL_ShowCursor (int toggle) {
	int ret = rSDL_ShowCursor(toggle);
	update_grab();
	return ret;
}

void SDLCALL SDL_WarpMouse (Uint16 x, Uint16 y) {
	if (main_window) rSDL_WarpMouseInWindow(main_window, x, y);
}

int SDLCALL SDL_GetGammaRamp (Uint16 *redtable, Uint16 *greentable, Uint16 *bluetable) {
	if (main_window) return rSDL_GetWindowGammaRamp(main_window, redtable, greentable, bluetable);
	else return -1;
}

int SDLCALL SDL_SetGammaRamp (Uint16 *redtable, Uint16 *greentable, Uint16 *bluetable) {
	if (main_window) return rSDL_SetWindowGammaRamp(main_window, redtable, greentable, bluetable);
	else return -1;
}

int SDLCALL SDL_SetGamma (float redgamma, float greengamma, float bluegamma) {
	Uint16 redtable[256], greentable[256], bluetable[256];
	rSDL_CalculateGammaRamp(redgamma, redtable);
	rSDL_CalculateGammaRamp(greengamma, greentable);
	rSDL_CalculateGammaRamp(bluegamma, bluetable);
	return SDL_SetGammaRamp(redtable, greentable, bluetable);
}

int SDLCALL SDL_GL_LoadLibrary (const char *path) {
	int ret = rSDL_GL_LoadLibrary(path);
	if (ret) printf("Error loading GL: %s", rSDL_GetError());
	return ret;
}

void *SDLCALL SDL_GL_GetProcAddress (const char *proc) {
	return rSDL_GL_GetProcAddress(proc);
}

void SDLCALL SDL_GL_SwapBuffers (void) {
	rSDL_GL_SwapWindow(main_window);
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

int SDLCALL SDL_GL_SetAttribute (SDL1_GLattr attr, int value) {
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

void SDLCALL SDL_WM_SetCaption (const char *title, const char *icon) {
	(void)title;
	(void)icon;
}


void SDLCALL SDL_WM_SetIcon (SDL_Surface *icon, Uint8 *mask) {
	(void)icon;
	(void)mask;
}

typedef enum {
	SDL1_GRAB_QUERY = -1,
	SDL1_GRAB_OFF = 0,
	SDL1_GRAB_ON = 1,
	SDL1_GRAB_FULLSCREEN
} SDL1_GrabMode;

SDL1_GrabMode SDLCALL SDL_WM_GrabInput (SDL1_GrabMode mode) {
	SDL1_GrabMode ret = grab ? SDL1_GRAB_ON : SDL1_GRAB_OFF;
	switch (mode) {
		case SDL1_GRAB_OFF: grab = SDL_FALSE; break;
		case SDL1_GRAB_ON: grab = SDL_TRUE; break;
		default: break;
	}
	update_grab();
	return ret;
}

int SDLCALL SDL_WM_IconifyWindow (void) {
	rSDL_MinimizeWindow(main_window);
	return 1;
}
