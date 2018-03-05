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
#include "events.h"

#define SDL1_BUTTON(X)        (1 << ((X)-1))
#define SDL1_BUTTON_LEFT      1
#define SDL1_BUTTON_MIDDLE    2
#define SDL1_BUTTON_RIGHT     3
#define SDL1_BUTTON_WHEELUP   4
#define SDL1_BUTTON_WHEELDOWN 5
#define SDL1_BUTTON_X1        6
#define SDL1_BUTTON_X2        7
#define SDL1_BUTTON_LMASK     SDL1_BUTTON(SDL1_BUTTON_LEFT)
#define SDL1_BUTTON_MMASK     SDL1_BUTTON(SDL1_BUTTON_MIDDLE)
#define SDL1_BUTTON_RMASK     SDL1_BUTTON(SDL1_BUTTON_RIGHT)
#define SDL1_BUTTON_X1MASK    SDL1_BUTTON(SDL1_BUTTON_X1)
#define SDL1_BUTTON_X2MASK    SDL1_BUTTON(SDL1_BUTTON_X2)

static Sint16 mouse_x = 0;
static Sint16 mouse_y = 0;
static Sint16 delta_x = 0;
static Sint16 delta_y = 0;
static Sint16 max_x = 0;
static Sint16 max_y = 0;
static Sint16 offset_x = 0;
static Sint16 offset_y = 0;
static Uint8 button_state = 0;

static Uint8 mousebutton2to1 (Uint8 button) {
	switch (button) {
		case SDL_BUTTON_LEFT: return SDL1_BUTTON_LEFT;
		case SDL_BUTTON_MIDDLE: return SDL1_BUTTON_MIDDLE;
		case SDL_BUTTON_RIGHT: return SDL1_BUTTON_RIGHT;
		case SDL_BUTTON_X1: return SDL1_BUTTON_X1;
		case SDL_BUTTON_X2: return SDL1_BUTTON_X2;
		default: return 0;
	}
}

static Uint8 mousestate2to1 (Uint32 state) {
	Uint8 state1 = 0;
	if (state & SDL_BUTTON_LMASK) state1 |= SDL1_BUTTON_LMASK;
	if (state & SDL_BUTTON_MMASK) state1 |= SDL1_BUTTON_MMASK;
	if (state & SDL_BUTTON_RMASK) state1 |= SDL1_BUTTON_RMASK;
	if (state & SDL_BUTTON_X1MASK) state1 |= SDL1_BUTTON_X1MASK;
	if (state & SDL_BUTTON_X2MASK) state1 |= SDL1_BUTTON_X2MASK;
	return state1;
}

DECLSPEC Uint8 SDLCALL SDL_GetMouseState (int *x, int *y) {
	*x = mouse_x;
	*y = mouse_y;
	return button_state;
}

DECLSPEC Uint8 SDLCALL SDL_GetRelativeMouseState (int *x, int *y) {
	*x = delta_x;
	*y = delta_y;
	delta_x = delta_y = 0;
	return button_state;
}

void SDLCALL SDLCL_SetMouseRange (int x, int y) {
	max_x = x;
	max_y = y;
	offset_x = offset_y = 0;
	mouse_x = mouse_y = 0;
}

void SDLCALL SDLCL_GetMouseOffset (int *x, int *y) {
	*x = offset_x;
	*y = offset_y;
}

static void update_position (Sint16 x, Sint16 y) {
	if (x >= offset_x + max_x) offset_x = x - max_x + 1;
	if (y >= offset_y + max_y) offset_y = y - max_y + 1;
	if (x < offset_x) offset_x = x;
	if (y < offset_y) offset_y = y;
	mouse_x = x - offset_x;
	mouse_y = y - offset_y;
}

void SDLCALL SDLCL_ProcessMouseMotion (SDL_Event *event2) {
	SDL1_Event event;
	update_position(event2->motion.x, event2->motion.y);
	delta_x += event2->motion.xrel;
	delta_y += event2->motion.yrel;
	button_state = mousestate2to1(event2->motion.state);
	event.motion.type = SDL1_MOUSEMOTION;
	event.motion.which = 0;
	event.motion.state = button_state;
	event.motion.x = mouse_x;
	event.motion.y = mouse_y;
	event.motion.xrel = event2->motion.xrel;
	event.motion.yrel = event2->motion.yrel;
	SDLCL_ProcessEvent(&event);
}

void SDLCALL SDLCL_ProcessMouseButton (SDL_Event *event2) {
	Uint8 button = mousebutton2to1(event2->button.button);
	Uint8 mask = SDL1_BUTTON(button);
	SDL1_Event event;
	update_position(event2->button.x, event2->button.y);
	if (event2->button.state == SDL_PRESSED) button_state |= mask;
	else button_state &= ~mask;
	event.button.type = (event2->type == SDL_MOUSEBUTTONDOWN) ? SDL1_MOUSEBUTTONDOWN : SDL1_MOUSEBUTTONUP;
	event.button.which = 0;
	event.button.button = button;
	event.button.state = (event2->button.state == SDL_PRESSED) ? SDL1_PRESSED : SDL1_RELEASED;
	event.button.x = mouse_x;
	event.button.y = mouse_y;
	SDLCL_ProcessEvent(&event);
}

void SDLCALL SDLCL_ProcessMouseWheel (SDL_Event *event2) {
	SDL1_Event event;
	if (event2->wheel.y == 0) return;
	event.button.type = SDL1_MOUSEBUTTONDOWN;
	event.button.which = 0;
	event.button.button = (event2->wheel.y > 0) ? SDL1_BUTTON_WHEELUP : SDL1_BUTTON_WHEELDOWN;
	event.button.state = SDL1_PRESSED;
	event.button.x = mouse_x;
	event.button.y = mouse_y;
	SDLCL_ProcessEvent(&event);
	event.button.type = SDL1_MOUSEBUTTONUP;
	event.button.state = SDL1_RELEASED;
	SDLCL_ProcessEvent(&event);
}
