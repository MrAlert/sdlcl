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

#define SDL1_ICONV_ERROR  (size_t)-1
#define SDL1_ICONV_E2BIG  (size_t)-2
#define SDL1_ICONV_EILSEQ (size_t)-3
#define SDL1_ICONV_EINVAL (size_t)-4

#ifdef HAVE_ICONV
/* Need to do this because SDL 1.x uses macros for SDL_iconv_open and SDL_iconv_close when iconv is available. */
#include <iconv.h>
#include <errno.h>
#if defined(_XGP6) || defined(__GLIBC__) && ((__GLIBC__ > 2) || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 2))
#define ICONV_INBUF_NONCONST
#endif

typedef iconv_t SDL1_iconv_t;

#define SDL_iconv_open iconv_open
#define SDL_iconv_close iconv_close

DECLSPEC size_t SDLCALL SDL_iconv (SDL1_iconv_t cd, const char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft) {
	size_t retCode;
#ifdef ICONV_INBUF_NONCONST
	retCode = iconv(cd, (char **)inbuf, inbytesleft, outbuf, outbytesleft);
#else
	retCode = iconv(cd, inbuf, inbytesleft, outbuf, outbytesleft);
#endif
	if (retCode == (size_t)-1) {
		switch (errno) {
			case E2BIG: return SDL1_ICONV_E2BIG;
			case EILSEQ: return SDL1_ICONV_EILSEQ;
			case EINVAL: return SDL1_ICONV_EINVAL;
			default: return SDL1_ICONV_ERROR;
		}
	}
	return retCode;
}
#else
/* SDL 1.x uses an opaque pointer like SDL 2.0 when iconv is unavailable. */
typedef SDL_iconv_t SDL1_iconv_t;

DECLSPEC SDL1_iconv_t SDLCALL SDL_iconv_open (const char *tocode, const char *fromcode) {
	return rSDL_iconv_open(tocode, fromcode);
}

DECLSPEC int SDLCALL SDL_iconv_close (SDL1_iconv_t cd) {
	return rSDL_iconv_close(cd);
}

DECLSPEC size_t SDLCALL SDL_iconv (SDL1_iconv_t cd, const char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft) {
	size_t ret = rSDL_iconv(cd, inbuf, inbytesleft, outbuf, outbytesleft);
	switch (ret) {
		case SDL_ICONV_ERROR: return SDL1_ICONV_ERROR;
		case SDL_ICONV_E2BIG: return SDL1_ICONV_E2BIG;
		case SDL_ICONV_EILSEQ: return SDL1_ICONV_EILSEQ;
		case SDL_ICONV_EINVAL: return SDL1_ICONV_EINVAL;
		default: return ret;
	}
}
#endif

DECLSPEC char *SDLCALL SDL_iconv_string (const char *tocode, const char *fromcode, const char *inbuf, size_t inbytesleft) {
	SDL1_iconv_t cd;
	char *string;
	size_t stringsize;
	char *outbuf;
	size_t outbytesleft;
	size_t retCode = 0;

	cd = SDL_iconv_open(tocode, fromcode);
	if ( cd == (SDL1_iconv_t)-1 ) {
		/* See if we can recover here (fixes iconv on Solaris 11) */
		if ( !tocode || !*tocode ) {
			tocode = "UTF-8";
		}
		if ( !fromcode || !*fromcode ) {
			fromcode = "UTF-8";
		}
		cd = SDL_iconv_open(tocode, fromcode);
	}
	if ( cd == (SDL1_iconv_t)-1 ) {
		return NULL;
	}

	stringsize = inbytesleft > 4 ? inbytesleft : 4;
	string = malloc(stringsize);
	if ( !string ) {
		SDL_iconv_close(cd);
		return NULL;
	}
	outbuf = string;
	outbytesleft = stringsize;
	memset(outbuf, 0, 4);

	while ( inbytesleft > 0 ) {
		retCode = SDL_iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
		switch (retCode) {
		    case SDL_ICONV_E2BIG:
			{
				char *oldstring = string;
				stringsize *= 2;
				string = realloc(string, stringsize);
				if ( !string ) {
					SDL_iconv_close(cd);
					return NULL;
				}
				outbuf = string + (outbuf - oldstring);
				outbytesleft = stringsize - (outbuf - string);
				memset(outbuf, 0, 4);
			}
			break;
		    case SDL_ICONV_EILSEQ:
			/* Try skipping some input data - not perfect, but... */
			++inbuf;
			--inbytesleft;
			break;
		    case SDL_ICONV_EINVAL:
		    case SDL_ICONV_ERROR:
			/* We can't continue... */
			inbytesleft = 0;
			break;
		}
	}
	SDL_iconv_close(cd);

	return string;
}
