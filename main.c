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

#define SDL2_SYMBOL(name, ret, param) ret (SDLCALL *r##name) param __attribute__ ((visibility ("hidden"))) = NULL;
#include "symbols.x"
#undef SDL2_SYMBOL

static void load_error (const char *symbol);

__attribute__ ((destructor)) static void quitlib (void) {
	if (lib) {
		dlclose(lib);
		lib = NULL;
#define SDL2_SYMBOL(name, ret, param) \
		r##name = NULL;
#include "symbols.x"
#undef SDL2_SYMBOL
	}
}

__attribute__ ((constructor)) static void initlib (void) {
	if (!lib) {
		lib = dlopen("libSDL2-2.0.so.0", RTLD_LAZY | RTLD_LOCAL);
		if (!lib) {
			load_error(NULL);
			return;
		}
#define SDL2_SYMBOL(name, ret, param) \
		r##name = dlsym(lib, #name); \
		if (!r##name) { \
			quitlib(); \
			load_error(#name); \
			return; \
		}
#include "symbols.x"
#undef SDL2_SYMBOL
	}
}

static Uint32 initflags1to2 (Uint32 flags) {
	Uint32 flags2 = 0;
	if (flags & SDL1_INIT_TIMER)    flags2 |= SDL_INIT_TIMER;
	if (flags & SDL1_INIT_AUDIO)    flags2 |= SDL_INIT_AUDIO;
	if (flags & SDL1_INIT_VIDEO)    flags2 |= SDL_INIT_VIDEO;
	if (flags & SDL1_INIT_JOYSTICK) flags2 |= SDL_INIT_JOYSTICK;
	return flags2;
}

int SDLCALL SDL_Init (Uint32 flags) {
	if (!lib) return -1;
	return rSDL_Init(initflags1to2(flags));
}

int SDLCALL SDL_InitSubSystem (Uint32 flags) {
	if (!lib) return -1;
	return rSDL_InitSubSystem(initflags1to2(flags));
}

void SDLCALL SDL_QuitSubSystem (Uint32 flags) {
	if (lib) rSDL_QuitSubSystem(initflags1to2(flags));
}

void SDLCALL SDL_Quit (void) {
	if (lib) rSDL_Quit();
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
static char errbuf[ERRBUF_SIZE];

char *SDLCALL SDL_GetError (void) {
	if (lib) {
		strncpy(errbuf, rSDL_GetError(), ERRBUF_SIZE);
		errbuf[ERRBUF_SIZE - 1] = 0;
	}
	return errbuf;
}

void SDLCALL SDL_SetError (const char *fmt, ...) {
	char err[ERRBUF_SIZE];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(err, ERRBUF_SIZE, fmt, ap);
	va_end(ap);
	if (lib) rSDL_SetError("%s", err);
	else strcpy(errbuf, err);
}

void SDLCALL SDL_ClearError (void) {
	if (lib) rSDL_ClearError();
	else errbuf[0] = 0;
}

void SDLCALL SDL_Error (SDL1_errorcode code) {
	SDL_errorcode code2;
	switch (code) {
		case SDL1_ENOMEM: code2 = SDL_ENOMEM; break;
		case SDL1_EFREAD: code2 = SDL_EFREAD; break;
		case SDL1_EFWRITE: code2 = SDL_EFWRITE; break;
		case SDL1_EFSEEK: code2 = SDL_EFSEEK; break;
		case SDL1_UNSUPPORTED:
		default:
			code2 = SDL1_UNSUPPORTED;
			break;
	}
	if (lib) rSDL_Error(code2);
	else SDL_SetError("SDL error code %d", (int)code);
}

static void load_error (const char *symbol) {
	if (symbol) SDL_SetError("Error loading SDL2 symbol %s", symbol);
	else SDL_SetError("Error loading SDL2: %s", dlerror());
}
