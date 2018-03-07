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
extern void SDLCL_UpdateGrab (void);

extern DECLSPEC void SDLCALL SDL_SetCursor (SDL1_Cursor *cursor);
extern DECLSPEC SDL1_Cursor *SDLCALL SDL_GetCursor (void);
extern DECLSPEC int SDLCALL SDL_ShowCursor (int toggle);

#endif
