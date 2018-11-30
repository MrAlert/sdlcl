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

#include <dlfcn.h>

#include "SDL2.h"
#include "loadso.h"
#include "main.h"

DECLSPEC void *SDLCALL SDL_LoadObject (const char *sofile) {
	void *handle = dlopen(sofile, RTLD_NOW);
	const char *loaderror;
	if (!handle) {
		loaderror = (char *)dlerror();
		SDL_SetError("Failed loading %s: %s", sofile, loaderror);
	}
	return handle;
}

DECLSPEC void *SDLCALL SDL_LoadFunction (void *handle, const char *name) {
	void *symbol = dlsym(handle, name);
	const char *loaderror;
	if (!symbol) {
		loaderror = (char *)dlerror();
		SDL_SetError("Failed loading %s: %s", name, loaderror);
	}
	return symbol;
}

DECLSPEC void SDLCALL SDL_UnloadObject (void *handle) {
	if (handle) dlclose(handle);
}
