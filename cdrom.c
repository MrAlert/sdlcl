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

#include "SDL2.h"

#define SDL1_MAX_TRACKS 99

#define SDL1_AUDIO_TRACK 0x00
#define SDL1_DATA_TRACK  0x04

typedef enum {
	CD1_TRAYEMPTY,
	CD1_STOPPED,
	CD1_PLAYING,
	CD1_PAUSED,
	CD1_ERROR = -1
} CD1status;

typedef struct SDL1_CDtrack {
	Uint8 id;
	Uint8 type;
	Uint16 unused;
	Uint32 length;
	Uint32 offset;
} SDL1_CDtrack;

typedef struct SDL1_CD {
	int id;
	CD1status status;
	int numtracks;
	int cur_track;
	int cur_frame;
	SDL1_CDtrack track[SDL1_MAX_TRACKS + 1];
} SDL1_CD;

int SDLCALL SDL_CDNumDrives (void) {
	return 0;
}

const char *SDLCALL SDL_CDName (int drive) {
	(void)drive;
	return NULL;
}

SDL1_CD *SDLCALL SDL_CDOpen (int drive) {
	(void)drive;
	return NULL;
}

CD1status SDLCALL SDL_CDStatus (SDL1_CD *cdrom) {
	(void)cdrom;
	return CD1_ERROR;
}

int SDLCALL SDL_CDPlayTracks (SDL1_CD *cdrom, int start_track, int start_frame, int ntracks, int nframes) {
	(void)cdrom;
	(void)start_track;
	(void)start_frame;
	(void)ntracks;
	(void)nframes;
	return -1;
}

int SDLCALL SDL_CDPlay (SDL1_CD *cdrom, int start, int length) {
	(void)cdrom;
	(void)start;
	(void)length;
	return -1;
}

int SDLCALL SDL_CDPause (SDL1_CD *cdrom) {
	(void)cdrom;
	return -1;
}

int SDLCALL SDL_CDResume (SDL1_CD *cdrom) {
	(void)cdrom;
	return -1;
}

int SDLCALL SDL_CDStop (SDL1_CD *cdrom) {
	(void)cdrom;
	return -1;
}

int SDLCALL SDL_CDEject (SDL1_CD *cdrom) {
	(void)cdrom;
	return -1;
}

void SDLCALL SDL_CDClose (SDL1_CD *cdrom) {
	(void)cdrom;
}
