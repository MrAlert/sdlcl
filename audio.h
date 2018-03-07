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

#ifndef SDLCL_AUDIO_H
#define SDLCL_AUDIO_H

#define AUDIO1_U8     0x0008
#define AUDIO1_S8     0x8008
#define AUDIO1_U16LSB 0x0010
#define AUDIO1_S16LSB 0x8010
#define AUDIO1_U16MSB 0x1010
#define AUDIO1_S16MSB 0x9010
#define AUDIO1_U16    AUDIO1_U16LSB
#define AUDIO1_S16    AUDIO1_S16LSB

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define AUDIO1_U16SYS  AUDIO1_U16LSB
#define AUDIO1_S16SYS  AUDIO1_S16LSB
#else
#define AUDIO1_U16SYS  AUDIO1_U16MSB
#define AUDIO1_S16SYS  AUDIO1_S16MSB
#endif

extern DECLSPEC int SDLCALL SDL_AudioInit (const char *driver_name);
extern DECLSPEC void SDLCALL SDL_AudioQuit (void);

#endif
