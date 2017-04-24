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

#ifndef SDLCL_RWOPS_H
#define SDLCL_RWOPS_H

#include "SDL2.h"

#define RW1_SEEK_SET 0
#define RW1_SEEK_CUR 1
#define RW1_SEEK_END 2

#define SDL1_RWseek(ctx, offset, whence) (ctx)->seek(ctx, offset, whence)
#define SDL1_RWtell(ctx)                 (ctx)->seek(ctx, 0, RW1_SEEK_CUR)
#define SDL1_RWread(ctx, ptr, size, n)   (ctx)->read(ctx, ptr, size, n)
#define SDL1_RWwrite(ctx, ptr, size, n)  (ctx)->write(ctx, ptr, size, n)
#define SDL1_RWclose(ctx)                (ctx)->close(ctx)

typedef struct SDL1_RWops {
	int (SDLCALL *seek)(struct SDL1_RWops *context, int offset, int whence);
	int (SDLCALL *read)(struct SDL1_RWops *context, void *ptr, int size, int maxnum);
	int (SDLCALL *write)(struct SDL1_RWops *context, const void *ptr, int size, int num);
	int (SDLCALL *close)(struct SDL1_RWops *context);
	Uint32 type;
	union {
		struct {
			int autoclose;
			FILE *fp;
		} stdio;
		struct {
			Uint8 *base;
			Uint8 *here;
			Uint8 *stop;
		} mem;
		struct {
			void *data1;
		} unknown;
	} hidden;
} SDL1_RWops;

extern SDL_RWops *SDLCALL SDLCL_RWFromSDL1 (SDL1_RWops *rwops);

#endif
