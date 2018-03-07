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
#include "events.h"

static SDL1_Cursor *current_cursor = NULL;
static SDL1_Cursor *default_cursor = NULL;
static int cursor_showing = SDL1_ENABLE;

static void init_cursor (void);

DECLSPEC void SDLCALL SDL_SetCursor (SDL1_Cursor *cursor) {
	if (cursor && cursor != current_cursor) {
		rSDL_SetCursor(cursor->sdl2);
		current_cursor = cursor;
	}
	if (!default_cursor) init_cursor();
}

DECLSPEC SDL1_Cursor *SDLCALL SDL_GetCursor (void) {
	return current_cursor;
}

DECLSPEC SDL1_Cursor *SDLCALL SDL_CreateCursor (Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y) {
	Uint8 d, m;
	Uint8 *p;
	int bw = w / 8;
	int bytes = bw * h;
	int i, j;
	SDL1_Cursor *cursor;
	if (w % 8) return NULL;
	cursor = malloc(sizeof(SDL1_Cursor) + bytes * 2 + w * h * 2);
	if (!cursor) return NULL;
	cursor->area.x = cursor->area.y = 0;
	cursor->area.w = w;
	cursor->area.h = h;
	cursor->hot_x = hot_x;
	cursor->hot_y = hot_y;
	cursor->data = (Uint8 *)(cursor + 1);
	cursor->mask = cursor->data + bytes;
	cursor->image = cursor->mask + bytes;
	memcpy(cursor->data, data, bytes);
	memcpy(cursor->mask, mask, bytes);
	p = cursor->image;
	for (i = 0; i < bytes; i++) {
		d = data[i];
		m = mask[i];
		for (j = 0; j < 8; j++) {
			*p++ = (!(d & 0x80) && (m & 0x80)) ? 255 : 0;
			*p++ = ((d & 0x80) || (m & 0x80)) ? 255 : 0;
			d <<= 1;
			m <<= 1;
		}
	}
	cursor->sdl2 = rSDL_CreateCursor(cursor->data, cursor->mask, cursor->area.w, cursor->area.h, cursor->hot_x, cursor->hot_y);
	return cursor;
}

static void init_cursor (void) {
	static Uint8 data[] = {
		0x00, 0x00, 0x10, 0x00, 0x18, 0x00, 0x1c, 0x00,
		0x1e, 0x00, 0x1f, 0x00, 0x1f, 0x80, 0x1f, 0xc0,
		0x1f, 0xe0, 0x1f, 0x00, 0x1b, 0x00, 0x11, 0x80,
		0x01, 0x80, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0x00
	};
	static Uint8 mask[] = {
		0x30, 0x00, 0x38, 0x00, 0x3c, 0x00, 0x3e, 0x00,
		0x3f, 0x00, 0x3f, 0x80, 0x3f, 0xc0, 0x3f, 0xe0,
		0x3f, 0xf0, 0x3f, 0xf0, 0x3f, 0x80, 0x3b, 0xc0,
		0x33, 0xc0, 0x01, 0xe0, 0x01, 0xe0, 0x00, 0xc0
	};
	static int w = 16;
	static int h = 16;
	static int hot_x = 3;
	static int hot_y = 1;
	if (!default_cursor) default_cursor = SDL_CreateCursor(data, mask, w, h, hot_x, hot_y);
	if (!current_cursor) SDL_SetCursor(default_cursor);
}

DECLSPEC void SDLCALL SDL_FreeCursor (SDL1_Cursor *cursor) {
	if (cursor) {
		if (cursor == current_cursor) SDL_SetCursor(default_cursor);
		rSDL_FreeCursor(cursor->sdl2);
		free(cursor);
	}
}

DECLSPEC int SDLCALL SDL_ShowCursor (int toggle) {
	int ret = cursor_showing;
	if (toggle != SDL1_QUERY) {
		cursor_showing = toggle;
		SDLCL_UpdateGrab();
	}
	return ret;
}

DECLSPEC void SDLCALL SDL_WarpMouse (Uint16 x, Uint16 y) {
	int offx, offy;
	if (SDLCL_window) {
		SDLCL_GetMouseOffset(&offx, &offy);
		rSDL_WarpMouseInWindow(SDLCL_window, x + offx, y + offy);
	}
}
