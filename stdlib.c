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

DECLSPEC char *SDLCALL SDL_ltoa (long value, char *string, int radix) {
	return rSDL_ltoa(value, string, radix);
}

DECLSPEC char *SDLCALL SDL_ultoa (unsigned long value, char *string, int radix) {
	return rSDL_ultoa(value, string, radix);
}

DECLSPEC char *SDLCALL SDL_lltoa (Sint64 value, char *string, int radix) {
	return rSDL_lltoa(value, string, radix);
}

DECLSPEC char *SDLCALL SDL_ulltoa (Uint64 value, char *string, int radix) {
	return rSDL_ulltoa(value, string, radix);
}

DECLSPEC size_t SDLCALL SDL_strlcpy (char *dst, const char *src, size_t maxlen) {
	return rSDL_strlcpy(dst, src, maxlen);
}

DECLSPEC size_t SDLCALL SDL_strlcat (char *dst, const char *src, size_t maxlen) {
	return rSDL_strlcat(dst, src, maxlen);
}

DECLSPEC char *SDLCALL SDL_strrev (char *string) {
	return rSDL_strrev(string);
}

DECLSPEC char *SDLCALL SDL_strupr (char *string) {
	return rSDL_strupr(string);
}

DECLSPEC char *SDLCALL SDL_strlwr (char *string) {
	return rSDL_strlwr(string);
}

DECLSPEC void *SDLCALL SDL_revcpy (void *dst, const void *src, size_t len) {
	const char *srcp = (const char *)src;
	char *dstp = (char *)dst;
	srcp += len - 1;
	dstp += len - 1;
	while (len--) *dstp-- = *srcp--;
	return dst;
}
