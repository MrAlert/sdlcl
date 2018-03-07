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

int SDLCALL SDLCL_JoystickInit (void) {
	return rSDL_InitSubSystem(SDL_INIT_JOYSTICK);
}

void SDLCALL SDLCL_JoystickQuit (void) {
	return rSDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

typedef struct SDL1_Joystick SDL1_Joystick;

struct SDL1_Joystick {
	int index;
	SDL_Joystick *sdl2;
	SDL1_Joystick *next;
};

static SDL1_Joystick *joystick_list = NULL;

DECLSPEC int SDLCALL SDL_NumJoysticks (void) {
	return rSDL_NumJoysticks();
}

DECLSPEC SDL1_Joystick *SDLCALL SDL_JoystickOpen (int index) {
	SDL1_Joystick *joystick;
	SDL_Joystick *sdl2 = rSDL_JoystickOpen(index);
	if (!sdl2) return NULL;
	joystick = malloc(sizeof(SDL1_Joystick));
	if (!joystick) {
		rSDL_JoystickClose(sdl2);
		return NULL;
	}
	joystick->index = index;
	joystick->sdl2 = sdl2;
	joystick->next = joystick_list;
	joystick_list = joystick;
	return joystick;
}

DECLSPEC const char *SDLCALL SDL_JoystickName (int index) {
	return rSDL_JoystickNameForIndex(index);
}

DECLSPEC int SDLCALL SDL_JoystickOpened (int index) {
	SDL1_Joystick *joystick;
	for (joystick = joystick_list; joystick != NULL; joystick = joystick->next)
		if (joystick->index == index) return 1;
	return 0;
}

DECLSPEC int SDLCALL SDL_JoystickIndex (SDL1_Joystick *joystick) {
	return joystick->index;
}

DECLSPEC int SDLCALL SDL_JoystickNumAxes (SDL1_Joystick *joystick) {
	return rSDL_JoystickNumAxes(joystick->sdl2);
}

DECLSPEC int SDLCALL SDL_JoystickNumBalls (SDL1_Joystick *joystick) {
	return rSDL_JoystickNumBalls(joystick->sdl2);
}

DECLSPEC int SDLCALL SDL_JoystickNumButtons (SDL1_Joystick *joystick) {
	return rSDL_JoystickNumButtons(joystick->sdl2);
}

DECLSPEC int SDLCALL SDL_JoystickNumHats (SDL1_Joystick *joystick) {
	return rSDL_JoystickNumHats(joystick->sdl2);
}

DECLSPEC Sint16 SDLCALL SDL_JoystickGetAxis (SDL1_Joystick *joystick, int axis) {
	return rSDL_JoystickGetAxis(joystick->sdl2, axis);
}

DECLSPEC int SDLCALL SDL_JoystickGetBall (SDL1_Joystick *joystick, int ball, int *dx, int *dy) {
	return rSDL_JoystickGetBall(joystick->sdl2, ball, dx, dy);
}

DECLSPEC Uint8 SDLCALL SDL_JoystickGetButton (SDL1_Joystick *joystick, int button) {
	return rSDL_JoystickGetButton(joystick->sdl2, button);
}

DECLSPEC Uint8 SDLCALL SDL_JoystickGetHat (SDL1_Joystick *joystick, int hat) {
	return rSDL_JoystickGetHat(joystick->sdl2, hat);
}

DECLSPEC void SDLCALL SDL_JoystickUpdate (void) {
	rSDL_JoystickUpdate();
}

DECLSPEC void SDLCALL SDL_JoystickClose (SDL1_Joystick *joystick) {
	SDL1_Joystick **ptr;
	rSDL_JoystickClose(joystick->sdl2);
	for (ptr = &joystick_list; ptr != NULL; ptr = &((*ptr)->next)) {
		if (*ptr == joystick) {
			*ptr = (*ptr)->next;
			break;
		}
	}
	free(joystick);
}
