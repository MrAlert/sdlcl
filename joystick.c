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

int SDLCALL SDL_NumJoysticks (void) {
	return rSDL_NumJoysticks();
}

SDL_Joystick *SDLCALL SDL_JoystickOpen (int index) {
	return rSDL_JoystickOpen(index);
}

const char *SDLCALL SDL_JoystickName (int index) {
	return rSDL_JoystickNameForIndex(index);
}

int SDLCALL SDL_JoystickNumAxes (SDL_Joystick *joystick) {
	return rSDL_JoystickNumAxes(joystick);
}

int SDLCALL SDL_JoystickNumBalls (SDL_Joystick *joystick) {
	return rSDL_JoystickNumBalls(joystick);
}

int SDLCALL SDL_JoystickNumButtons (SDL_Joystick *joystick) {
	return rSDL_JoystickNumButtons(joystick);
}

int SDLCALL SDL_JoystickNumHats (SDL_Joystick *joystick) {
	return rSDL_JoystickNumHats(joystick);
}

Sint16 SDLCALL SDL_JoystickGetAxis (SDL_Joystick *joystick, int axis) {
	return rSDL_JoystickGetAxis(joystick, axis);
}

int SDLCALL SDL_JoystickGetBall (SDL_Joystick *joystick, int ball, int *dx, int *dy) {
	return rSDL_JoystickGetBall(joystick, ball, dx, dy);
}

Uint8 SDLCALL SDL_JoystickGetButton (SDL_Joystick *joystick, int button) {
	return rSDL_JoystickGetButton(joystick, button);
}

Uint8 SDLCALL SDL_JoystickGetHat (SDL_Joystick *joystick, int hat) {
	return rSDL_JoystickGetHat(joystick, hat);
}

void SDLCALL SDL_JoystickUpdate (void) {
	rSDL_JoystickUpdate();
}

void SDLCALL SDL_JoystickClose (SDL_Joystick *joystick) {
	return rSDL_JoystickClose(joystick);
}
