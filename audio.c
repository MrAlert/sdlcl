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
#include <stdio.h>

#include "SDL2.h"

typedef struct SDL1_AudioSpec {
	int freq;
	Uint16 format;
	Uint8 channels;
	Uint8 silence;
	Uint16 samples;
	Uint16 padding;
	Uint32 size;
	void (SDLCALL *callback)(void *userdata, Uint8 *stream, int len);
	void *userdata;
} SDL1_AudioSpec;

#define AUDIO1_U8     0x0008
#define AUDIO1_S8     0x8008
#define AUDIO1_U16LSB 0x0010
#define AUDIO1_S16LSB 0x8010
#define AUDIO1_U16MSB 0x1010
#define AUDIO1_S16MSB 0x9010
#define AUDIO1_U16    AUDIO1_U16LSB
#define AUDIO1_S16    AUDIO1_S16LSB

typedef struct callback_data {
	void (SDLCALL *callback)(void *userdata, Uint8 *stream, int len);
	void *userdata;
} callback_data;

static callback_data cbdata;
static SDL_AudioSpec audio_spec;

static void SDLCALL callback (void *userdata, Uint8 *stream, int len) {
	(void)userdata;
	memset(stream, audio_spec.silence, len);
	cbdata.callback(cbdata.userdata, stream, len);
}

int SDLCALL SDL_OpenAudio (SDL1_AudioSpec *desired, SDL1_AudioSpec *obtained) {
	SDL_AudioSpec desired2;
	memset(&desired2, 0, sizeof(SDL_AudioSpec));
	desired2.freq = desired->freq;
	desired2.format = desired->format;
	desired2.channels = desired->channels;
	desired2.samples = desired->samples;
	desired2.callback = callback;
	desired2.userdata = NULL;
	if (obtained) {
		if (!rSDL_OpenAudio(&desired2, &audio_spec)) {
			cbdata.callback = desired->callback;
			cbdata.userdata = desired->userdata;
			obtained->freq = audio_spec.freq;
			obtained->format = audio_spec.format;
			obtained->channels = audio_spec.channels;
			obtained->samples = audio_spec.samples;
			obtained->size = audio_spec.size;
			obtained->silence = audio_spec.silence;
			return 0;
		}
	} else {
		if (!rSDL_OpenAudio(&desired2, NULL)) {
			cbdata.callback = desired->callback;
			cbdata.userdata = desired->userdata;
			return 0;
		}
	}
	return -1;
}

void SDLCALL SDL_PauseAudio (int pause_on) {
	rSDL_PauseAudio(pause_on);
}

void SDLCALL SDL_LockAudio (void) {
	rSDL_LockAudio();
}

void SDLCALL SDL_UnlockAudio (void) {
	rSDL_UnlockAudio();
}

void SDLCALL SDL_CloseAudio (void) {
	rSDL_CloseAudio();
}

char *SDLCALL SDL_AudioDriverName (char *namebuf, int maxlen) {
	const char *name = rSDL_GetCurrentAudioDriver();
	if (!name) return NULL;
	strncpy(namebuf, name, maxlen);
	namebuf[maxlen - 1] = 0;
	return namebuf;
}
