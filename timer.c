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

#include <stdint.h>

#include "SDL2.h"

int SDLCALL SDLCL_TimerInit (void) {
	return rSDL_InitSubSystem(SDL_INIT_TIMER);
}

void SDLCALL SDLCL_TimerQuit (void) {
	rSDL_QuitSubSystem(SDL_INIT_TIMER);
}

DECLSPEC Uint32 SDLCALL SDL_GetTicks (void) {
	return rSDL_GetTicks();
}

DECLSPEC void SDLCALL SDL_Delay (Uint32 ms) {
	rSDL_Delay(ms);
}

typedef Uint32 (SDLCALL *SDL1_TimerCallback)(Uint32 interval);

static SDL_TimerID old_timer = 0;

static Uint32 SDLCALL old_timer_callback (Uint32 interval, void *param) {
	SDL1_TimerCallback callback = param;
	return callback(interval);
}

DECLSPEC int SDLCALL SDL_SetTimer (Uint32 interval, SDL1_TimerCallback callback) {
	if (old_timer) {
		rSDL_RemoveTimer(old_timer);
		old_timer = 0;
	}
	if (interval && callback) {
		old_timer = rSDL_AddTimer(interval, old_timer_callback, callback);
		return old_timer ? 0 : -1;
	} else
		return 0;
}

typedef Uint32 (SDLCALL *SDL1_NewTimerCallback)(Uint32 interval, void *param);
typedef struct _SDL1_TimerID *SDL1_TimerID;

/* Use uintptr_t as an intermediate type for pointer->int->pointer conversions to quiet compiler warnings. */
/* Since we don't dereference these pointers, this should be safe. */
DECLSPEC SDL1_TimerID SDLCALL SDL_AddTimer (Uint32 interval, SDL1_NewTimerCallback callback, void *param) {
	return (SDL1_TimerID)(uintptr_t)rSDL_AddTimer(interval, callback, param);
}

DECLSPEC SDL_bool SDLCALL SDL_RemoveTimer(SDL1_TimerID t) {
	return rSDL_RemoveTimer((SDL_TimerID)(uintptr_t)t);
}
