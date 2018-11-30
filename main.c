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

#include "SDL2.h"
#include "video.h"
#include "audio.h"
#include "loadso.h"

#define SDL1_INIT_TIMER       0x00000001
#define SDL1_INIT_AUDIO       0x00000010
#define SDL1_INIT_VIDEO       0x00000020
#define SDL1_INIT_CDROM       0x00000100
#define SDL1_INIT_JOYSTICK    0x00000200
#define SDL1_INIT_NOPARACHUTE 0x00100000
#define SDL1_INIT_EVENTTHREAD 0x01000000
#define SDL1_INIT_EVERYTHING  0x0000FFFF

static void *lib = NULL;

#define SDL2_SYMBOL(name, ret, param) ret (SDLCALL *r##name) param = NULL;
#include "symbols.x"
#undef SDL2_SYMBOL

__attribute__ ((destructor)) static void quitlib (void) {
	if (lib) {
		SDL_UnloadObject(lib);
		lib = NULL;
	}
#define SDL2_SYMBOL(name, ret, param) \
	r##name = NULL;
#include "symbols.x"
#undef SDL2_SYMBOL
}

__attribute__ ((constructor)) static void initlib (void) {
	void *handle;
	if (!lib) {
		handle = SDL_LoadObject("libSDL2-2.0.so.0");
		if (!handle) {
			return;
		}
#define SDL2_SYMBOL(name, ret, param) \
		r##name = SDL_LoadFunction(handle, #name); \
		if (!r##name) { \
			SDL_UnloadObject(handle); \
			quitlib(); \
			return; \
		}
#include "symbols.x"
#undef SDL2_SYMBOL
		lib = handle;
	}
}

extern int SDLCALL SDLCL_TimerInit (void);
extern void SDLCALL SDLCL_TimerQuit (void);

extern int SDLCALL SDLCL_JoystickInit (void);
extern void SDLCALL SDLCL_JoystickQuit (void);

static Uint32 initialized = 0;

DECLSPEC int SDLCALL SDL_InitSubSystem (Uint32 flags) {
	Uint32 needinit = flags & ~initialized;
	if (!lib) return -1;
	if (needinit & SDL1_INIT_TIMER) {
		if (SDLCL_TimerInit() < 0) return -1;
		initialized |= SDL1_INIT_TIMER;
	}
	if (needinit & SDL1_INIT_VIDEO) {
		if (SDL_VideoInit(NULL, 0) < 0) return -1;
		initialized |= SDL1_INIT_VIDEO;
	}
	if (needinit & SDL1_INIT_AUDIO) {
		if (SDL_AudioInit(NULL) < 0) return -1;
		initialized |= SDL1_INIT_AUDIO;
	}
	if (needinit & SDL1_INIT_JOYSTICK) {
		if (SDLCL_JoystickInit() < 0) return -1;
		initialized |= SDL1_INIT_JOYSTICK;
	}
	if (needinit & SDL1_INIT_CDROM) {
		/* CD-ROM subsystem is stubbed */
		initialized |= SDL1_INIT_CDROM;
	}
	return 0;
}

DECLSPEC int SDLCALL SDL_Init (Uint32 flags) {
	if (!lib) return -1;
	if (rSDL_Init(0)) return -1;
	return SDL_InitSubSystem(flags);
}

DECLSPEC void SDLCALL SDL_QuitSubSystem (Uint32 flags) {
	Uint32 needquit = flags & initialized;
	if (!lib) return;
	if (needquit & SDL1_INIT_CDROM) {
		/* CD-ROM subsystem is stubbed */
		initialized &= ~SDL1_INIT_CDROM;
	}
	if (needquit & SDL1_INIT_JOYSTICK) {
		SDLCL_JoystickQuit();
		initialized &= ~SDL1_INIT_JOYSTICK;
	}
	if (needquit & SDL1_INIT_AUDIO) {
		SDL_AudioQuit();
		initialized &= ~SDL1_INIT_AUDIO;
	}
	if (needquit & SDL1_INIT_VIDEO) {
		SDL_VideoQuit();
		initialized &= ~SDL1_INIT_VIDEO;
	}
	if (needquit & SDL1_INIT_TIMER) {
		SDLCL_TimerQuit();
		initialized &= ~SDL1_INIT_TIMER;
	}
}

DECLSPEC Uint32 SDLCALL SDL_WasInit (Uint32 flags) {
	if (!lib) return 0;
	if (!flags) flags = SDL1_INIT_EVERYTHING;
	return initialized & flags;
}

DECLSPEC void SDLCALL SDL_Quit (void) {
	SDL_QuitSubSystem(SDL1_INIT_EVERYTHING);
	if (lib) rSDL_Quit();
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

DECLSPEC char *SDLCALL SDL_GetError (void) {
	if (lib) {
		strncpy(errbuf, rSDL_GetError(), ERRBUF_SIZE);
		errbuf[ERRBUF_SIZE - 1] = 0;
	}
	return errbuf;
}

DECLSPEC void SDLCALL SDL_SetError (const char *fmt, ...) {
	char err[ERRBUF_SIZE];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(err, ERRBUF_SIZE, fmt, ap);
	va_end(ap);
	if (lib) rSDL_SetError("%s", err);
	else strcpy(errbuf, err);
}

DECLSPEC void SDLCALL SDL_ClearError (void) {
	if (lib) rSDL_ClearError();
	else errbuf[0] = 0;
}

DECLSPEC void SDLCALL SDL_Error (SDL1_errorcode code) {
	SDL_errorcode code2;
	switch (code) {
		case SDL1_ENOMEM: code2 = SDL_ENOMEM; break;
		case SDL1_EFREAD: code2 = SDL_EFREAD; break;
		case SDL1_EFWRITE: code2 = SDL_EFWRITE; break;
		case SDL1_EFSEEK: code2 = SDL_EFSEEK; break;
		case SDL1_UNSUPPORTED:
		default:
			code2 = SDL_UNSUPPORTED;
			break;
	}
	if (lib) rSDL_Error(code2);
	else SDL_SetError("SDL error code %d", (int)code);
}
