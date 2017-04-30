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
#include <dlfcn.h>

#include "SDL2.h"

#define SDL1_INIT_TIMER       0x00000001
#define SDL1_INIT_AUDIO       0x00000010
#define SDL1_INIT_VIDEO       0x00000020
#define SDL1_INIT_CDROM       0x00000100
#define SDL1_INIT_JOYSTICK    0x00000200
#define SDL1_INIT_NOPARACHUTE 0x00100000
#define SDL1_INIT_EVENTTHREAD 0x01000000
#define SDL1_INIT_EVERYTHING  0x0000FFFF

static void *lib = NULL;

#define SDL2_SYMBOL(name, ret, param) ret (SDLCALL *r##name) param __attribute__ ((visibility ("hidden")));
#include "symbols.x"
#undef SDL2_SYMBOL

static Uint32 initflags1to2 (Uint32 flags) {
	Uint32 flags2 = 0;
	if (flags & SDL1_INIT_TIMER)    flags2 |= SDL_INIT_TIMER;
	if (flags & SDL1_INIT_AUDIO)    flags2 |= SDL_INIT_AUDIO;
	if (flags & SDL1_INIT_VIDEO)    flags2 |= SDL_INIT_VIDEO;
	if (flags & SDL1_INIT_JOYSTICK) flags2 |= SDL_INIT_JOYSTICK;
	return flags2;
}

static int initlib (void) {
	if (!lib) {
		lib = dlopen("libSDL2-2.0.so.0", RTLD_LAZY);
		if (!lib) return -1;
#define SDL2_SYMBOL(name, ret, param) \
		r##name = dlsym(lib, #name); \
		if (!r##name) { \
			dlclose(lib); \
			lib = NULL; \
			return -1; \
		}
#include "symbols.x"
#undef SDL2_SYMBOL
	}
	return 0;
}

int SDLCALL SDL_Init (Uint32 flags) {
	if (initlib()) return -1;
	return rSDL_Init(initflags1to2(flags));
}

int SDLCALL SDL_InitSubSystem (Uint32 flags) {
	if (initlib()) return -1;
	return rSDL_InitSubSystem(initflags1to2(flags));
}

void SDLCALL SDL_QuitSubSystem (Uint32 flags) {
	rSDL_QuitSubSystem(initflags1to2(flags));
}

void SDLCALL SDL_Quit (void) {
	if (lib) {
		rSDL_Quit();
		dlclose(lib);
		lib = NULL;
	}
}

Uint32 SDLCALL SDL_WasInit (Uint32 flags) {
	Uint32 ret2, ret = 0;
	if (!lib) return 0;
	ret2 = rSDL_WasInit(initflags1to2(flags));
	if (ret2 & SDL_INIT_TIMER) ret |= SDL1_INIT_TIMER;
	if (ret2 & SDL_INIT_AUDIO) ret |= SDL1_INIT_AUDIO;
	if (ret2 & SDL_INIT_VIDEO) ret |= SDL1_INIT_VIDEO;
	if (ret2 & SDL_INIT_JOYSTICK) ret |= SDL1_INIT_JOYSTICK;
	return ret;
}

typedef enum {
	SDL1_ENOMEM,
	SDL1_EFREAD,
	SDL1_EFWRITE,
	SDL1_EFSEEK,
	SDL1_UNSUPPORTED,
	SDL1_LASTERROR
} SDL1_errorcode;

#define ERRBUF_SIZE 1024
char *SDLCALL SDL_GetError (void) {
	static char errbuf[ERRBUF_SIZE];
	static const char *err;
	if (lib) {
		err = rSDL_GetError();
	} else {
		err = "Error loading SDL";
	}
	strncpy(errbuf, err, ERRBUF_SIZE);
	errbuf[ERRBUF_SIZE - 1] = 0;
	return errbuf;
}

/* TODO: Implement these error-setting functions */
void SDLCALL SDL_SetError (const char *fmt, ...) {
	(void)fmt;
}

void SDLCALL SDL_ClearError (void) {
}

void SDLCALL SDL_Error (SDL1_errorcode code) {
	(void)code;
}
