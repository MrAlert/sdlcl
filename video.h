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

#ifndef SDLCL_VIDEO_H
#define SDLCL_VIDEO_H

#include "SDL2.h"

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

typedef struct SDL1_Cursor {
	SDL1_Rect area;
	Sint16 hot_x, hot_y;
	Uint8 *data;
	Uint8 *mask;
	/* Originally Uint8 *save[2]; */
	Uint8 *image;
	Uint8 *pad;
	/* Originally WMcursor *wm_cursor; */
	SDL_Cursor *sdl2;
} SDL1_Cursor;

extern SDL_Window *SDLCL_window;
extern SDL_Renderer *SDLCL_renderer;
extern SDL1_Surface *SDLCL_surface;
extern int SDLCL_scaling;
extern int SDLCL_virtual_width;
extern int SDLCL_virtual_height;
extern SDL_Rect SDLCL_scale_rect;
extern void SDLCL_UpdateGrab (void);

extern DECLSPEC int SDLCALL SDL_VideoInit (const char *driver_name, Uint32 flags);
extern DECLSPEC void SDLCALL SDL_VideoQuit (void);

extern DECLSPEC void SDLCALL SDL_SetCursor (SDL1_Cursor *cursor);
extern DECLSPEC SDL1_Cursor *SDLCALL SDL_GetCursor (void);
extern DECLSPEC int SDLCALL SDL_ShowCursor (int toggle);

#endif
