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

extern DECLSPEC SDL_bool SDLCALL SDL_HasRDTSC(void)
{
	return rSDL_HasRDTSC();
}

extern DECLSPEC SDL_bool SDLCALL SDL_HasMMX(void)
{
	return rSDL_HasMMX();
}

extern DECLSPEC SDL_bool SDLCALL SDL_HasMMXExt(void)
{
	// Dirty hack. Won't detect non-SSE, MMX-Extended AMDs.
	return rSDL_HasMMX() && rSDL_HasSSE();
}

extern DECLSPEC SDL_bool SDLCALL SDL_Has3DNow(void)
{
	return rSDL_Has3DNow();
}

extern DECLSPEC SDL_bool SDLCALL SDL_Has3DNowExt(void)
{
	//TODO: implement?
	return SDL_FALSE;
}

extern DECLSPEC SDL_bool SDLCALL SDL_HasSSE(void)
{
	return rSDL_HasSSE();
}

extern DECLSPEC SDL_bool SDLCALL SDL_HasSSE2(void)
{
	return rSDL_HasSSE2();
}

extern DECLSPEC SDL_bool SDLCALL SDL_HasAltiVec(void)
{
	return rSDL_HasAltiVec();
}
