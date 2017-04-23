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
#include <stdio.h>

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

SDL1_RWops *SDLCALL SDL_AllocRW (void) {
	return malloc(sizeof(SDL1_RWops));
}

void SDLCALL SDL_FreeRW (SDL1_RWops *area) {
	free(area);
}

static int SDLCALL stdio_seek (SDL1_RWops *context, int offset, int whence) {
	if (fseek(context->hidden.stdio.fp, offset, whence) == 0) {
		return ftell(context->hidden.stdio.fp);
	} else {
		//SDL_Error(SDL_EFSEEK);
		return -1;
	}
}

static int SDLCALL stdio_read (SDL1_RWops *context, void *ptr, int size, int maxnum) {
	size_t nread;
	nread = fread(ptr, size, maxnum, context->hidden.stdio.fp);
	if (nread == 0 && ferror(context->hidden.stdio.fp)) {
		//SDL_Error(SDL_EFREAD);
	}
	return nread;
}

static int SDLCALL stdio_write (SDL1_RWops *context, const void *ptr, int size, int num) {
	size_t nwrote;
	nwrote = fwrite(ptr, size, num, context->hidden.stdio.fp);
	if (nwrote == 0 && ferror(context->hidden.stdio.fp)) {
		//SDL_Error(SDL_EFWRITE);
	}
	return nwrote;
}

static int SDLCALL stdio_close (SDL1_RWops *context) {
	if (context) {
		if (context->hidden.stdio.autoclose) {
			/* WARNING: Check the return value here! */
			fclose(context->hidden.stdio.fp);
		}
		SDL_FreeRW(context);
	}
	return 0;
}

static int SDLCALL mem_seek (SDL1_RWops *context, int offset, int whence) {
	Uint8 *newpos;
	switch (whence) {
		case RW1_SEEK_SET:
			newpos = context->hidden.mem.base + offset;
			break;
		case RW1_SEEK_CUR:
			newpos = context->hidden.mem.here + offset;
			break;
		case RW1_SEEK_END:
			newpos = context->hidden.mem.stop + offset;
			break;
		default:
			//SDL_SetError("Unknown value for 'whence'");
			return -1;
	}
	if (newpos < context->hidden.mem.base) newpos = context->hidden.mem.base;
	if (newpos > context->hidden.mem.stop) newpos = context->hidden.mem.stop;
	context->hidden.mem.here = newpos;
	return context->hidden.mem.here - context->hidden.mem.base;
}

static int SDLCALL mem_read (SDL1_RWops *context, void *ptr, int size, int maxnum) {
	size_t total_bytes;
	size_t mem_available;
	total_bytes = (maxnum * size);
	if ((maxnum <= 0) || (size <= 0) || ((total_bytes / maxnum) != (size_t)size))
		return 0;
	mem_available = context->hidden.mem.stop - context->hidden.mem.here;
	if (total_bytes > mem_available) total_bytes = mem_available;
	memcpy(ptr, context->hidden.mem.here, total_bytes);
	context->hidden.mem.here += total_bytes;
	return total_bytes / size;
}

static int SDLCALL mem_write (SDL1_RWops *context, const void *ptr, int size, int num) {
	if (context->hidden.mem.here + num * size > context->hidden.mem.stop)
		num = (context->hidden.mem.stop - context->hidden.mem.here) / size;
	memcpy(context->hidden.mem.here, ptr, num * size);
	context->hidden.mem.here += num * size;
	return num;
}

static int SDLCALL mem_writeconst (SDL1_RWops *context, const void *ptr, int size, int num) {
	(void)context;
	(void)ptr;
	(void)size;
	(void)num;
	//SDL_SetError("Can't write to read-only memory");
	return -1;
}

static int SDLCALL mem_close (SDL1_RWops *context) {
	if (context) {
		SDL_FreeRW(context);
	}
	return 0;
}

SDL1_RWops *SDLCALL SDL_RWFromFP (FILE *fp, int autoclose) {
	SDL1_RWops *rwops;
	rwops = SDL_AllocRW();
	if (rwops) {
		rwops->seek = stdio_seek;
		rwops->read = stdio_read;
		rwops->write = stdio_write;
		rwops->close = stdio_close;
		rwops->hidden.stdio.fp = fp;
		rwops->hidden.stdio.autoclose = autoclose;
	}
	return rwops;
}

SDL1_RWops *SDLCALL SDL_RWFromFile (const char *file, const char *mode) {
	SDL1_RWops *rwops = NULL;
	FILE *fp = NULL;
	(void)fp;
	if (!file || !*file || !mode || !*mode) {
		//SDL_SetError("SDL_RWFromFile(): No file or no mode specified");
		return NULL;
	}
	fp = fopen(file, mode);
	if (!fp) {
		//SDL_SetError("Couldn't open %s", file);
	} else {
		rwops = SDL_RWFromFP(fp, 1);
	}
	return rwops;
}

SDL1_RWops *SDLCALL SDL_RWFromMem (void *mem, int size) {
	SDL1_RWops *rwops;
	rwops = SDL_AllocRW();
	if (rwops) {
		rwops->seek = mem_seek;
		rwops->read = mem_read;
		rwops->write = mem_write;
		rwops->close = mem_close;
		rwops->hidden.mem.base = (Uint8 *)mem;
		rwops->hidden.mem.here = rwops->hidden.mem.base;
		rwops->hidden.mem.stop = rwops->hidden.mem.base + size;
	}
	return rwops;
}

SDL1_RWops *SDLCALL SDL_RWFromConstMem (const void *mem, int size) {
	SDL1_RWops *rwops;
	rwops = SDL_AllocRW();
	if (rwops) {
		rwops->seek = mem_seek;
		rwops->read = mem_read;
		rwops->write = mem_writeconst;
		rwops->close = mem_close;
		rwops->hidden.mem.base = (Uint8 *)mem;
		rwops->hidden.mem.here = rwops->hidden.mem.base;
		rwops->hidden.mem.stop = rwops->hidden.mem.base + size;
	}
	return rwops;
}

#define ReadBits(E,B) \
Uint##B SDLCALL SDL_Read##E##B (SDL1_RWops *src) { \
	Uint##B value; \
	SDL1_RWread(src, &value, sizeof(value), 1); \
	return SDL_Swap##E##B(value); \
}

ReadBits(LE, 16)
ReadBits(BE, 16)
ReadBits(LE, 32)
ReadBits(BE, 32)
ReadBits(LE, 64)
ReadBits(BE, 64)

#define WriteBits(E,B) \
int SDLCALL SDL_Write##E##B (SDL1_RWops *dst, Uint##B value) { \
	value = SDL_Swap##E##B(value); \
	return SDL1_RWwrite(dst, &value, sizeof(value), 1); \
}

WriteBits(LE, 16)
WriteBits(BE, 16)
WriteBits(LE, 32)
WriteBits(BE, 32)
WriteBits(LE, 64)
WriteBits(BE, 64)
