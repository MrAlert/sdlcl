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

#include <string.h>

#include "SDL2.h"
#include "events.h"

static int key_delay = 0;
static int key_interval = 0;

DECLSPEC int SDLCALL SDL_EnableKeyRepeat (int delay, int interval) {
	key_delay = delay;
	key_interval = interval;
	return 0;
}

DECLSPEC void SDL_GetKeyRepeat (int *delay, int *interval) {
	*delay = key_delay;
	*interval = key_interval;
}

static SDL1Mod keymod2to1 (SDL_Keymod keymod) {
	SDL1Mod ret = KMOD1_NONE;
	if (keymod & KMOD_LSHIFT) ret |= KMOD1_LSHIFT;
	if (keymod & KMOD_RSHIFT) ret |= KMOD1_RSHIFT;
	if (keymod & KMOD_LCTRL)  ret |= KMOD1_LCTRL;
	if (keymod & KMOD_RCTRL)  ret |= KMOD1_RCTRL;
	if (keymod & KMOD_LALT)   ret |= KMOD1_LALT;
	if (keymod & KMOD_RALT)   ret |= KMOD1_RALT;
	if (keymod & KMOD_LGUI)   ret |= KMOD1_LMETA;
	if (keymod & KMOD_RGUI)   ret |= KMOD1_RMETA;
	if (keymod & KMOD_NUM)    ret |= KMOD1_NUM;
	if (keymod & KMOD_CAPS)   ret |= KMOD1_CAPS;
	if (keymod & KMOD_MODE)   ret |= KMOD1_MODE;
	return ret;
}

DECLSPEC SDL1Mod SDLCALL SDL_GetModState (void) {
	return keymod2to1(rSDL_GetModState());
}

static SDL_Keymod keymod1to2 (SDL1Mod keymod) {
	SDL_Keymod ret = KMOD_NONE;
	if (keymod & KMOD1_LSHIFT) ret |= KMOD_LSHIFT;
	if (keymod & KMOD1_RSHIFT) ret |= KMOD_RSHIFT;
	if (keymod & KMOD1_LCTRL)  ret |= KMOD_LCTRL;
	if (keymod & KMOD1_RCTRL)  ret |= KMOD_RCTRL;
	if (keymod & KMOD1_LALT)   ret |= KMOD_LALT;
	if (keymod & KMOD1_RALT)   ret |= KMOD_RALT;
	if (keymod & KMOD1_LMETA)  ret |= KMOD_LGUI;
	if (keymod & KMOD1_RMETA)  ret |= KMOD_RGUI;
	if (keymod & KMOD1_NUM)    ret |= KMOD_NUM;
	if (keymod & KMOD1_CAPS)   ret |= KMOD_CAPS;
	if (keymod & KMOD1_MODE)   ret |= KMOD_MODE;
	return ret;
}

DECLSPEC void SDLCALL SDL_SetModState (SDL1Mod modstate) {
	rSDL_SetModState(keymod1to2(modstate));
}

static Uint8 key_state[SDLK1_LAST] = { 0 };

DECLSPEC Uint8 *SDLCALL SDL_GetKeyState (int *numkeys) {
	if (numkeys) *numkeys = SDLK1_LAST;
	return key_state;
}

static SDL_Keycode key1to2 (SDL1Key key) {
	SDL_Keycode ret;
	switch (key) {
		default: ret = SDLK_UNKNOWN; break;
		case SDLK1_BACKSPACE: ret = SDLK_BACKSPACE; break;
		case SDLK1_TAB: ret = SDLK_TAB; break;
		case SDLK1_RETURN: ret = SDLK_RETURN; break;
		case SDLK1_ESCAPE: ret = SDLK_ESCAPE; break;
		case SDLK1_SPACE: ret = SDLK_SPACE; break;
		case SDLK1_EXCLAIM: ret = SDLK_EXCLAIM; break;
		case SDLK1_QUOTEDBL: ret = SDLK_QUOTEDBL; break;
		case SDLK1_HASH: ret = SDLK_HASH; break;
		case SDLK1_DOLLAR: ret = SDLK_DOLLAR; break;
		//case SDLK1_PERCENT: ret = SDLK_PERCENT; break;
		case SDLK1_AMPERSAND: ret = SDLK_AMPERSAND; break;
		case SDLK1_QUOTE: ret = SDLK_QUOTE; break;
		case SDLK1_LEFTPAREN: ret = SDLK_LEFTPAREN; break;
		case SDLK1_RIGHTPAREN: ret = SDLK_RIGHTPAREN; break;
		case SDLK1_ASTERISK: ret = SDLK_ASTERISK; break;
		case SDLK1_PLUS: ret = SDLK_PLUS; break;
		case SDLK1_COMMA: ret = SDLK_COMMA; break;
		case SDLK1_MINUS: ret = SDLK_MINUS; break;
		case SDLK1_PERIOD: ret = SDLK_PERIOD; break;
		case SDLK1_SLASH: ret = SDLK_SLASH; break;
		case SDLK1_0: ret = SDLK_0; break;
		case SDLK1_1: ret = SDLK_1; break;
		case SDLK1_2: ret = SDLK_2; break;
		case SDLK1_3: ret = SDLK_3; break;
		case SDLK1_4: ret = SDLK_4; break;
		case SDLK1_5: ret = SDLK_5; break;
		case SDLK1_6: ret = SDLK_6; break;
		case SDLK1_7: ret = SDLK_7; break;
		case SDLK1_8: ret = SDLK_8; break;
		case SDLK1_9: ret = SDLK_9; break;
		case SDLK1_COLON: ret = SDLK_COLON; break;
		case SDLK1_SEMICOLON: ret = SDLK_SEMICOLON; break;
		case SDLK1_LESS: ret = SDLK_LESS; break;
		case SDLK1_EQUALS: ret = SDLK_EQUALS; break;
		case SDLK1_GREATER: ret = SDLK_GREATER; break;
		case SDLK1_QUESTION: ret = SDLK_QUESTION; break;
		case SDLK1_AT: ret = SDLK_AT; break;
		case SDLK1_LEFTBRACKET: ret = SDLK_LEFTBRACKET; break;
		case SDLK1_BACKSLASH: ret = SDLK_BACKSLASH; break;
		case SDLK1_RIGHTBRACKET: ret = SDLK_RIGHTBRACKET; break;
		case SDLK1_CARET: ret = SDLK_CARET; break;
		case SDLK1_UNDERSCORE: ret = SDLK_UNDERSCORE; break;
		case SDLK1_BACKQUOTE: ret = SDLK_BACKQUOTE; break;
		case SDLK1_a: ret = SDLK_a; break;
		case SDLK1_b: ret = SDLK_b; break;
		case SDLK1_c: ret = SDLK_c; break;
		case SDLK1_d: ret = SDLK_d; break;
		case SDLK1_e: ret = SDLK_e; break;
		case SDLK1_f: ret = SDLK_f; break;
		case SDLK1_g: ret = SDLK_g; break;
		case SDLK1_h: ret = SDLK_h; break;
		case SDLK1_i: ret = SDLK_i; break;
		case SDLK1_j: ret = SDLK_j; break;
		case SDLK1_k: ret = SDLK_k; break;
		case SDLK1_l: ret = SDLK_l; break;
		case SDLK1_m: ret = SDLK_m; break;
		case SDLK1_n: ret = SDLK_n; break;
		case SDLK1_o: ret = SDLK_o; break;
		case SDLK1_p: ret = SDLK_p; break;
		case SDLK1_q: ret = SDLK_q; break;
		case SDLK1_r: ret = SDLK_r; break;
		case SDLK1_s: ret = SDLK_s; break;
		case SDLK1_t: ret = SDLK_t; break;
		case SDLK1_u: ret = SDLK_u; break;
		case SDLK1_v: ret = SDLK_v; break;
		case SDLK1_w: ret = SDLK_w; break;
		case SDLK1_x: ret = SDLK_x; break;
		case SDLK1_y: ret = SDLK_y; break;
		case SDLK1_z: ret = SDLK_z; break;
		case SDLK1_DELETE: ret = SDLK_DELETE; break;
		case SDLK1_CAPSLOCK: ret = SDLK_CAPSLOCK; break;
		case SDLK1_F1: ret = SDLK_F1; break;
		case SDLK1_F2: ret = SDLK_F2; break;
		case SDLK1_F3: ret = SDLK_F3; break;
		case SDLK1_F4: ret = SDLK_F4; break;
		case SDLK1_F5: ret = SDLK_F5; break;
		case SDLK1_F6: ret = SDLK_F6; break;
		case SDLK1_F7: ret = SDLK_F7; break;
		case SDLK1_F8: ret = SDLK_F8; break;
		case SDLK1_F9: ret = SDLK_F9; break;
		case SDLK1_F10: ret = SDLK_F10; break;
		case SDLK1_F11: ret = SDLK_F11; break;
		case SDLK1_F12: ret = SDLK_F12; break;
		case SDLK1_PRINT: ret = SDLK_PRINTSCREEN; break;
		case SDLK1_SCROLLOCK: ret = SDLK_SCROLLLOCK; break;
		case SDLK1_PAUSE: ret = SDLK_PAUSE; break;
		case SDLK1_INSERT: ret = SDLK_INSERT; break;
		case SDLK1_HOME: ret = SDLK_HOME; break;
		case SDLK1_PAGEUP: ret = SDLK_PAGEUP; break;
		case SDLK1_END: ret = SDLK_END; break;
		case SDLK1_PAGEDOWN: ret = SDLK_PAGEDOWN; break;
		case SDLK1_RIGHT: ret = SDLK_RIGHT; break;
		case SDLK1_LEFT: ret = SDLK_LEFT; break;
		case SDLK1_DOWN: ret = SDLK_DOWN; break;
		case SDLK1_UP: ret = SDLK_UP; break;
		//case SDLK1_NUMLOCKCLEAR: ret = SDLK_NUMLOCKCLEAR; break;
		case SDLK1_KP_DIVIDE: ret = SDLK_KP_DIVIDE; break;
		case SDLK1_KP_MULTIPLY: ret = SDLK_KP_MULTIPLY; break;
		case SDLK1_KP_MINUS: ret = SDLK_KP_MINUS; break;
		case SDLK1_KP_PLUS: ret = SDLK_KP_PLUS; break;
		case SDLK1_KP_ENTER: ret = SDLK_KP_ENTER; break;
		case SDLK1_KP1: ret = SDLK_KP_1; break;
		case SDLK1_KP2: ret = SDLK_KP_2; break;
		case SDLK1_KP3: ret = SDLK_KP_3; break;
		case SDLK1_KP4: ret = SDLK_KP_4; break;
		case SDLK1_KP5: ret = SDLK_KP_5; break;
		case SDLK1_KP6: ret = SDLK_KP_6; break;
		case SDLK1_KP7: ret = SDLK_KP_7; break;
		case SDLK1_KP8: ret = SDLK_KP_8; break;
		case SDLK1_KP9: ret = SDLK_KP_9; break;
		case SDLK1_KP0: ret = SDLK_KP_0; break;
		case SDLK1_KP_PERIOD: ret = SDLK_KP_PERIOD; break;
		//case SDLK1_APPLICATION: ret = SDLK_APPLICATION; break;
		case SDLK1_POWER: ret = SDLK_POWER; break;
		case SDLK1_KP_EQUALS: ret = SDLK_KP_EQUALS; break;
		case SDLK1_F13: ret = SDLK_F13; break;
		case SDLK1_F14: ret = SDLK_F14; break;
		case SDLK1_F15: ret = SDLK_F15; break;
		//case SDLK1_F16: ret = SDLK_F16; break;
		//case SDLK1_F17: ret = SDLK_F17; break;
		//case SDLK1_F18: ret = SDLK_F18; break;
		//case SDLK1_F19: ret = SDLK_F19; break;
		//case SDLK1_F20: ret = SDLK_F20; break;
		//case SDLK1_F21: ret = SDLK_F21; break;
		//case SDLK1_F22: ret = SDLK_F22; break;
		//case SDLK1_F23: ret = SDLK_F23; break;
		//case SDLK1_F24: ret = SDLK_F24; break;
		//case SDLK1_EXECUTE: ret = SDLK_EXECUTE; break;
		case SDLK1_HELP: ret = SDLK_HELP; break;
		case SDLK1_MENU: ret = SDLK_MENU; break;
		//case SDLK1_SELECT: ret = SDLK_SELECT; break;
		//case SDLK1_STOP: ret = SDLK_STOP; break;
		//case SDLK1_AGAIN: ret = SDLK_AGAIN; break;
		case SDLK1_UNDO: ret = SDLK_UNDO; break;
		//case SDLK1_CUT: ret = SDLK_CUT; break;
		//case SDLK1_COPY: ret = SDLK_COPY; break;
		//case SDLK1_PASTE: ret = SDLK_PASTE; break;
		//case SDLK1_FIND: ret = SDLK_FIND; break;
		//case SDLK1_MUTE: ret = SDLK_MUTE; break;
		//case SDLK1_VOLUMEUP: ret = SDLK_VOLUMEUP; break;
		//case SDLK1_VOLUMEDOWN: ret = SDLK_VOLUMEDOWN; break;
		//case SDLK1_KP_COMMA: ret = SDLK_KP_COMMA; break;
		//case SDLK1_KP_EQUALSAS400: ret = SDLK_KP_EQUALSAS400; break;
		//case SDLK1_ALTERASE: ret = SDLK_ALTERASE; break;
		case SDLK1_SYSREQ: ret = SDLK_SYSREQ; break;
		//case SDLK1_CANCEL: ret = SDLK_CANCEL; break;
		case SDLK1_CLEAR: ret = SDLK_CLEAR; break;
		//case SDLK1_PRIOR: ret = SDLK_PRIOR; break;
		//case SDLK1_RETURN2: ret = SDLK_RETURN2; break;
		//case SDLK1_SEPARATOR: ret = SDLK_SEPARATOR; break;
		//case SDLK1_OUT: ret = SDLK_OUT; break;
		//case SDLK1_OPER: ret = SDLK_OPER; break;
		//case SDLK1_CLEARAGAIN: ret = SDLK_CLEARAGAIN; break;
		//case SDLK1_CRSEL: ret = SDLK_CRSEL; break;
		//case SDLK1_EXSEL: ret = SDLK_EXSEL; break;
		//case SDLK1_KP_00: ret = SDLK_KP_00; break;
		//case SDLK1_KP_000: ret = SDLK_KP_000; break;
		//case SDLK1_THOUSANDSSEPARATOR: ret = SDLK_THOUSANDSSEPARATOR; break;
		//case SDLK1_DECIMALSEPARATOR: ret = SDLK_DECIMALSEPARATOR; break;
		//case SDLK1_CURRENCYUNIT: ret = SDLK_CURRENCYUNIT; break;
		//case SDLK1_CURRENCYSUBUNIT: ret = SDLK_CURRENCYSUBUNIT; break;
		//case SDLK1_KP_LEFTPAREN: ret = SDLK_KP_LEFTPAREN; break;
		//case SDLK1_KP_RIGHTPAREN: ret = SDLK_KP_RIGHTPAREN; break;
		//case SDLK1_KP_LEFTBRACE: ret = SDLK_KP_LEFTBRACE; break;
		//case SDLK1_KP_RIGHTBRACE: ret = SDLK_KP_RIGHTBRACE; break;
		//case SDLK1_KP_TAB: ret = SDLK_KP_TAB; break;
		//case SDLK1_KP_BACKSPACE: ret = SDLK_KP_BACKSPACE; break;
		//case SDLK1_KP_A: ret = SDLK_KP_A; break;
		//case SDLK1_KP_B: ret = SDLK_KP_B; break;
		//case SDLK1_KP_C: ret = SDLK_KP_C; break;
		//case SDLK1_KP_D: ret = SDLK_KP_D; break;
		//case SDLK1_KP_E: ret = SDLK_KP_E; break;
		//case SDLK1_KP_F: ret = SDLK_KP_F; break;
		//case SDLK1_KP_XOR: ret = SDLK_KP_XOR; break;
		//case SDLK1_KP_POWER: ret = SDLK_KP_POWER; break;
		//case SDLK1_KP_PERCENT: ret = SDLK_KP_PERCENT; break;
		//case SDLK1_KP_LESS: ret = SDLK_KP_LESS; break;
		//case SDLK1_KP_GREATER: ret = SDLK_KP_GREATER; break;
		//case SDLK1_KP_AMPERSAND: ret = SDLK_KP_AMPERSAND; break;
		//case SDLK1_KP_DBLAMPERSAND: ret = SDLK_KP_DBLAMPERSAND; break;
		//case SDLK1_KP_VERTICALBAR: ret = SDLK_KP_VERTICALBAR; break;
		//case SDLK1_KP_DBLVERTICALBAR: ret = SDLK_KP_DBLVERTICALBAR; break;
		//case SDLK1_KP_COLON: ret = SDLK_KP_COLON; break;
		//case SDLK1_KP_HASH: ret = SDLK_KP_HASH; break;
		//case SDLK1_KP_SPACE: ret = SDLK_KP_SPACE; break;
		//case SDLK1_KP_AT: ret = SDLK_KP_AT; break;
		//case SDLK1_KP_EXCLAM: ret = SDLK_KP_EXCLAM; break;
		//case SDLK1_KP_MEMSTORE: ret = SDLK_KP_MEMSTORE; break;
		//case SDLK1_KP_MEMRECALL: ret = SDLK_KP_MEMRECALL; break;
		//case SDLK1_KP_MEMCLEAR: ret = SDLK_KP_MEMCLEAR; break;
		//case SDLK1_KP_MEMADD: ret = SDLK_KP_MEMADD; break;
		//case SDLK1_KP_MEMSUBTRACT: ret = SDLK_KP_MEMSUBTRACT; break;
		//case SDLK1_KP_MEMMULTIPLY: ret = SDLK_KP_MEMMULTIPLY; break;
		//case SDLK1_KP_MEMDIVIDE: ret = SDLK_KP_MEMDIVIDE; break;
		//case SDLK1_KP_PLUSMINUS: ret = SDLK_KP_PLUSMINUS; break;
		//case SDLK1_KP_CLEAR: ret = SDLK_KP_CLEAR; break;
		//case SDLK1_KP_CLEARENTRY: ret = SDLK_KP_CLEARENTRY; break;
		//case SDLK1_KP_BINARY: ret = SDLK_KP_BINARY; break;
		//case SDLK1_KP_OCTAL: ret = SDLK_KP_OCTAL; break;
		//case SDLK1_KP_DECIMAL: ret = SDLK_KP_DECIMAL; break;
		//case SDLK1_KP_HEXADECIMAL: ret = SDLK_KP_HEXADECIMAL; break;
		case SDLK1_LCTRL: ret = SDLK_LCTRL; break;
		case SDLK1_LSHIFT: ret = SDLK_LSHIFT; break;
		case SDLK1_LALT: ret = SDLK_LALT; break;
		//case SDLK1_LGUI: ret = SDLK_LGUI; break;
		case SDLK1_RCTRL: ret = SDLK_RCTRL; break;
		case SDLK1_RSHIFT: ret = SDLK_RSHIFT; break;
		case SDLK1_RALT: ret = SDLK_RALT; break;
		//case SDLK1_RGUI: ret = SDLK_RGUI; break;
		case SDLK1_MODE: ret = SDLK_MODE; break;
		//case SDLK1_AUDIONEXT: ret = SDLK_AUDIONEXT; break;
		//case SDLK1_AUDIOPREV: ret = SDLK_AUDIOPREV; break;
		//case SDLK1_AUDIOSTOP: ret = SDLK_AUDIOSTOP; break;
		//case SDLK1_AUDIOPLAY: ret = SDLK_AUDIOPLAY; break;
		//case SDLK1_AUDIOMUTE: ret = SDLK_AUDIOMUTE; break;
		//case SDLK1_MEDIASELECT: ret = SDLK_MEDIASELECT; break;
		//case SDLK1_WWW: ret = SDLK_WWW; break;
		//case SDLK1_MAIL: ret = SDLK_MAIL; break;
		//case SDLK1_CALCULATOR: ret = SDLK_CALCULATOR; break;
		//case SDLK1_COMPUTER: ret = SDLK_COMPUTER; break;
		//case SDLK1_AC_SEARCH: ret = SDLK_AC_SEARCH; break;
		//case SDLK1_AC_HOME: ret = SDLK_AC_HOME; break;
		//case SDLK1_AC_BACK: ret = SDLK_AC_BACK; break;
		//case SDLK1_AC_FORWARD: ret = SDLK_AC_FORWARD; break;
		//case SDLK1_AC_STOP: ret = SDLK_AC_STOP; break;
		//case SDLK1_AC_REFRESH: ret = SDLK_AC_REFRESH; break;
		//case SDLK1_AC_BOOKMARKS: ret = SDLK_AC_BOOKMARKS; break;
		//case SDLK1_BRIGHTNESSDOWN: ret = SDLK_BRIGHTNESSDOWN; break;
		//case SDLK1_BRIGHTNESSUP: ret = SDLK_BRIGHTNESSUP; break;
		//case SDLK1_DISPLAYSWITCH: ret = SDLK_DISPLAYSWITCH; break;
		//case SDLK1_KBDILLUMTOGGLE: ret = SDLK_KBILLUMTOGGLE; break;
		//case SDLK1_KBDILLUMDOWN: ret = SDLK_KBDILLUMDOWN; break;
		//case SDLK1_KBDILLUMUP: ret = SDLK_KBDILLUMUP; break;
		//case SDLK1_EJECT: ret = SDLK_EJECT; break;
		//case SDLK1_SLEEP: ret = SDLK_SLEEP; break;
	}
	return ret;
}

#define KEYNAME_MAX 64

DECLSPEC char *SDL_GetKeyName (SDL1Key key) {
	static char buf[KEYNAME_MAX];
	const char *name = rSDL_GetKeyName(key1to2(key));
	strncpy(buf, name, KEYNAME_MAX);
	buf[KEYNAME_MAX - 1] = 0;
	return buf;
}

static SDL1_keysym keysym2to1 (SDL_Keysym keysym) {
	SDL1_keysym ret;
	/* Hardware scancodes are not available from SDL 2.0, so set it to 0. */
	ret.scancode = 0;
	ret.mod = keymod2to1(keysym.mod);
	/* Unicode translation handled elsewhere */
	ret.unicode = 0;
	switch (keysym.sym) {
		default: ret.sym = SDLK1_UNKNOWN; break;
		case SDLK_BACKSPACE: ret.sym = SDLK1_BACKSPACE; break;
		case SDLK_TAB: ret.sym = SDLK1_TAB; break;
		case SDLK_RETURN: ret.sym = SDLK1_RETURN; break;
		case SDLK_ESCAPE: ret.sym = SDLK1_ESCAPE; break;
		case SDLK_SPACE: ret.sym = SDLK1_SPACE; break;
		case SDLK_EXCLAIM: ret.sym = SDLK1_EXCLAIM; break;
		case SDLK_QUOTEDBL: ret.sym = SDLK1_QUOTEDBL; break;
		case SDLK_HASH: ret.sym = SDLK1_HASH; break;
		case SDLK_DOLLAR: ret.sym = SDLK1_DOLLAR; break;
		//case SDLK_PERCENT: ret.sym = SDLK1_PERCENT; break;
		case SDLK_AMPERSAND: ret.sym = SDLK1_AMPERSAND; break;
		case SDLK_QUOTE: ret.sym = SDLK1_QUOTE; break;
		case SDLK_LEFTPAREN: ret.sym = SDLK1_LEFTPAREN; break;
		case SDLK_RIGHTPAREN: ret.sym = SDLK1_RIGHTPAREN; break;
		case SDLK_ASTERISK: ret.sym = SDLK1_ASTERISK; break;
		case SDLK_PLUS: ret.sym = SDLK1_PLUS; break;
		case SDLK_COMMA: ret.sym = SDLK1_COMMA; break;
		case SDLK_MINUS: ret.sym = SDLK1_MINUS; break;
		case SDLK_PERIOD: ret.sym = SDLK1_PERIOD; break;
		case SDLK_SLASH: ret.sym = SDLK1_SLASH; break;
		case SDLK_0: ret.sym = SDLK1_0; break;
		case SDLK_1: ret.sym = SDLK1_1; break;
		case SDLK_2: ret.sym = SDLK1_2; break;
		case SDLK_3: ret.sym = SDLK1_3; break;
		case SDLK_4: ret.sym = SDLK1_4; break;
		case SDLK_5: ret.sym = SDLK1_5; break;
		case SDLK_6: ret.sym = SDLK1_6; break;
		case SDLK_7: ret.sym = SDLK1_7; break;
		case SDLK_8: ret.sym = SDLK1_8; break;
		case SDLK_9: ret.sym = SDLK1_9; break;
		case SDLK_COLON: ret.sym = SDLK1_COLON; break;
		case SDLK_SEMICOLON: ret.sym = SDLK1_SEMICOLON; break;
		case SDLK_LESS: ret.sym = SDLK1_LESS; break;
		case SDLK_EQUALS: ret.sym = SDLK1_EQUALS; break;
		case SDLK_GREATER: ret.sym = SDLK1_GREATER; break;
		case SDLK_QUESTION: ret.sym = SDLK1_QUESTION; break;
		case SDLK_AT: ret.sym = SDLK1_AT; break;
		case SDLK_LEFTBRACKET: ret.sym = SDLK1_LEFTBRACKET; break;
		case SDLK_BACKSLASH: ret.sym = SDLK1_BACKSLASH; break;
		case SDLK_RIGHTBRACKET: ret.sym = SDLK1_RIGHTBRACKET; break;
		case SDLK_CARET: ret.sym = SDLK1_CARET; break;
		case SDLK_UNDERSCORE: ret.sym = SDLK1_UNDERSCORE; break;
		case SDLK_BACKQUOTE: ret.sym = SDLK1_BACKQUOTE; break;
		case SDLK_a: ret.sym = SDLK1_a; break;
		case SDLK_b: ret.sym = SDLK1_b; break;
		case SDLK_c: ret.sym = SDLK1_c; break;
		case SDLK_d: ret.sym = SDLK1_d; break;
		case SDLK_e: ret.sym = SDLK1_e; break;
		case SDLK_f: ret.sym = SDLK1_f; break;
		case SDLK_g: ret.sym = SDLK1_g; break;
		case SDLK_h: ret.sym = SDLK1_h; break;
		case SDLK_i: ret.sym = SDLK1_i; break;
		case SDLK_j: ret.sym = SDLK1_j; break;
		case SDLK_k: ret.sym = SDLK1_k; break;
		case SDLK_l: ret.sym = SDLK1_l; break;
		case SDLK_m: ret.sym = SDLK1_m; break;
		case SDLK_n: ret.sym = SDLK1_n; break;
		case SDLK_o: ret.sym = SDLK1_o; break;
		case SDLK_p: ret.sym = SDLK1_p; break;
		case SDLK_q: ret.sym = SDLK1_q; break;
		case SDLK_r: ret.sym = SDLK1_r; break;
		case SDLK_s: ret.sym = SDLK1_s; break;
		case SDLK_t: ret.sym = SDLK1_t; break;
		case SDLK_u: ret.sym = SDLK1_u; break;
		case SDLK_v: ret.sym = SDLK1_v; break;
		case SDLK_w: ret.sym = SDLK1_w; break;
		case SDLK_x: ret.sym = SDLK1_x; break;
		case SDLK_y: ret.sym = SDLK1_y; break;
		case SDLK_z: ret.sym = SDLK1_z; break;
		case SDLK_DELETE: ret.sym = SDLK1_DELETE; break;
		case SDLK_CAPSLOCK: ret.sym = SDLK1_CAPSLOCK; break;
		case SDLK_F1: ret.sym = SDLK1_F1; break;
		case SDLK_F2: ret.sym = SDLK1_F2; break;
		case SDLK_F3: ret.sym = SDLK1_F3; break;
		case SDLK_F4: ret.sym = SDLK1_F4; break;
		case SDLK_F5: ret.sym = SDLK1_F5; break;
		case SDLK_F6: ret.sym = SDLK1_F6; break;
		case SDLK_F7: ret.sym = SDLK1_F7; break;
		case SDLK_F8: ret.sym = SDLK1_F8; break;
		case SDLK_F9: ret.sym = SDLK1_F9; break;
		case SDLK_F10: ret.sym = SDLK1_F10; break;
		case SDLK_F11: ret.sym = SDLK1_F11; break;
		case SDLK_F12: ret.sym = SDLK1_F12; break;
		case SDLK_PRINTSCREEN: ret.sym = SDLK1_PRINT; break;
		case SDLK_SCROLLLOCK: ret.sym = SDLK1_SCROLLOCK; break;
		case SDLK_PAUSE: ret.sym = SDLK1_PAUSE; break;
		case SDLK_INSERT: ret.sym = SDLK1_INSERT; break;
		case SDLK_HOME: ret.sym = SDLK1_HOME; break;
		case SDLK_PAGEUP: ret.sym = SDLK1_PAGEUP; break;
		case SDLK_END: ret.sym = SDLK1_END; break;
		case SDLK_PAGEDOWN: ret.sym = SDLK1_PAGEDOWN; break;
		case SDLK_RIGHT: ret.sym = SDLK1_RIGHT; break;
		case SDLK_LEFT: ret.sym = SDLK1_LEFT; break;
		case SDLK_DOWN: ret.sym = SDLK1_DOWN; break;
		case SDLK_UP: ret.sym = SDLK1_UP; break;
		//case SDLK_NUMLOCKCLEAR: ret.sym = SDLK1_NUMLOCKCLEAR; break;
		case SDLK_KP_DIVIDE: ret.sym = SDLK1_KP_DIVIDE; break;
		case SDLK_KP_MULTIPLY: ret.sym = SDLK1_KP_MULTIPLY; break;
		case SDLK_KP_MINUS: ret.sym = SDLK1_KP_MINUS; break;
		case SDLK_KP_PLUS: ret.sym = SDLK1_KP_PLUS; break;
		case SDLK_KP_ENTER: ret.sym = SDLK1_KP_ENTER; break;
		case SDLK_KP_1: ret.sym = SDLK1_KP1; break;
		case SDLK_KP_2: ret.sym = SDLK1_KP2; break;
		case SDLK_KP_3: ret.sym = SDLK1_KP3; break;
		case SDLK_KP_4: ret.sym = SDLK1_KP4; break;
		case SDLK_KP_5: ret.sym = SDLK1_KP5; break;
		case SDLK_KP_6: ret.sym = SDLK1_KP6; break;
		case SDLK_KP_7: ret.sym = SDLK1_KP7; break;
		case SDLK_KP_8: ret.sym = SDLK1_KP8; break;
		case SDLK_KP_9: ret.sym = SDLK1_KP9; break;
		case SDLK_KP_0: ret.sym = SDLK1_KP0; break;
		case SDLK_KP_PERIOD: ret.sym = SDLK1_KP_PERIOD; break;
		//case SDLK_APPLICATION: ret.sym = SDLK1_APPLICATION; break;
		case SDLK_POWER: ret.sym = SDLK1_POWER; break;
		case SDLK_KP_EQUALS: ret.sym = SDLK1_KP_EQUALS; break;
		case SDLK_F13: ret.sym = SDLK1_F13; break;
		case SDLK_F14: ret.sym = SDLK1_F14; break;
		case SDLK_F15: ret.sym = SDLK1_F15; break;
		//case SDLK_F16: ret.sym = SDLK1_F16; break;
		//case SDLK_F17: ret.sym = SDLK1_F17; break;
		//case SDLK_F18: ret.sym = SDLK1_F18; break;
		//case SDLK_F19: ret.sym = SDLK1_F19; break;
		//case SDLK_F20: ret.sym = SDLK1_F20; break;
		//case SDLK_F21: ret.sym = SDLK1_F21; break;
		//case SDLK_F22: ret.sym = SDLK1_F22; break;
		//case SDLK_F23: ret.sym = SDLK1_F23; break;
		//case SDLK_F24: ret.sym = SDLK1_F24; break;
		//case SDLK_EXECUTE: ret.sym = SDLK1_EXECUTE; break;
		case SDLK_HELP: ret.sym = SDLK1_HELP; break;
		case SDLK_MENU: ret.sym = SDLK1_MENU; break;
		//case SDLK_SELECT: ret.sym = SDLK1_SELECT; break;
		//case SDLK_STOP: ret.sym = SDLK1_STOP; break;
		//case SDLK_AGAIN: ret.sym = SDLK1_AGAIN; break;
		case SDLK_UNDO: ret.sym = SDLK1_UNDO; break;
		//case SDLK_CUT: ret.sym = SDLK1_CUT; break;
		//case SDLK_COPY: ret.sym = SDLK1_COPY; break;
		//case SDLK_PASTE: ret.sym = SDLK1_PASTE; break;
		//case SDLK_FIND: ret.sym = SDLK1_FIND; break;
		//case SDLK_MUTE: ret.sym = SDLK1_MUTE; break;
		//case SDLK_VOLUMEUP: ret.sym = SDLK1_VOLUMEUP; break;
		//case SDLK_VOLUMEDOWN: ret.sym = SDLK1_VOLUMEDOWN; break;
		//case SDLK_KP_COMMA: ret.sym = SDLK1_KP_COMMA; break;
		//case SDLK_KP_EQUALSAS400: ret.sym = SDLK1_KP_EQUALSAS400; break;
		//case SDLK_ALTERASE: ret.sym = SDLK1_ALTERASE; break;
		case SDLK_SYSREQ: ret.sym = SDLK1_SYSREQ; break;
		//case SDLK_CANCEL: ret.sym = SDLK1_CANCEL; break;
		case SDLK_CLEAR: ret.sym = SDLK1_CLEAR; break;
		//case SDLK_PRIOR: ret.sym = SDLK1_PRIOR; break;
		//case SDLK_RETURN2: ret.sym = SDLK1_RETURN2; break;
		//case SDLK_SEPARATOR: ret.sym = SDLK1_SEPARATOR; break;
		//case SDLK_OUT: ret.sym = SDLK1_OUT; break;
		//case SDLK_OPER: ret.sym = SDLK1_OPER; break;
		//case SDLK_CLEARAGAIN: ret.sym = SDLK1_CLEARAGAIN; break;
		//case SDLK_CRSEL: ret.sym = SDLK1_CRSEL; break;
		//case SDLK_EXSEL: ret.sym = SDLK1_EXSEL; break;
		//case SDLK_KP_00: ret.sym = SDLK1_KP_00; break;
		//case SDLK_KP_000: ret.sym = SDLK1_KP_000; break;
		//case SDLK_THOUSANDSSEPARATOR: ret.sym = SDLK1_THOUSANDSSEPARATOR; break;
		//case SDLK_DECIMALSEPARATOR: ret.sym = SDLK1_DECIMALSEPARATOR; break;
		//case SDLK_CURRENCYUNIT: ret.sym = SDLK1_CURRENCYUNIT; break;
		//case SDLK_CURRENCYSUBUNIT: ret.sym = SDLK1_CURRENCYSUBUNIT; break;
		//case SDLK_KP_LEFTPAREN: ret.sym = SDLK1_KP_LEFTPAREN; break;
		//case SDLK_KP_RIGHTPAREN: ret.sym = SDLK1_KP_RIGHTPAREN; break;
		//case SDLK_KP_LEFTBRACE: ret.sym = SDLK1_KP_LEFTBRACE; break;
		//case SDLK_KP_RIGHTBRACE: ret.sym = SDLK1_KP_RIGHTBRACE; break;
		//case SDLK_KP_TAB: ret.sym = SDLK1_KP_TAB; break;
		//case SDLK_KP_BACKSPACE: ret.sym = SDLK1_KP_BACKSPACE; break;
		//case SDLK_KP_A: ret.sym = SDLK1_KP_A; break;
		//case SDLK_KP_B: ret.sym = SDLK1_KP_B; break;
		//case SDLK_KP_C: ret.sym = SDLK1_KP_C; break;
		//case SDLK_KP_D: ret.sym = SDLK1_KP_D; break;
		//case SDLK_KP_E: ret.sym = SDLK1_KP_E; break;
		//case SDLK_KP_F: ret.sym = SDLK1_KP_F; break;
		//case SDLK_KP_XOR: ret.sym = SDLK1_KP_XOR; break;
		//case SDLK_KP_POWER: ret.sym = SDLK1_KP_POWER; break;
		//case SDLK_KP_PERCENT: ret.sym = SDLK1_KP_PERCENT; break;
		//case SDLK_KP_LESS: ret.sym = SDLK1_KP_LESS; break;
		//case SDLK_KP_GREATER: ret.sym = SDLK1_KP_GREATER; break;
		//case SDLK_KP_AMPERSAND: ret.sym = SDLK1_KP_AMPERSAND; break;
		//case SDLK_KP_DBLAMPERSAND: ret.sym = SDLK1_KP_DBLAMPERSAND; break;
		//case SDLK_KP_VERTICALBAR: ret.sym = SDLK1_KP_VERTICALBAR; break;
		//case SDLK_KP_DBLVERTICALBAR: ret.sym = SDLK1_KP_DBLVERTICALBAR; break;
		//case SDLK_KP_COLON: ret.sym = SDLK1_KP_COLON; break;
		//case SDLK_KP_HASH: ret.sym = SDLK1_KP_HASH; break;
		//case SDLK_KP_SPACE: ret.sym = SDLK1_KP_SPACE; break;
		//case SDLK_KP_AT: ret.sym = SDLK1_KP_AT; break;
		//case SDLK_KP_EXCLAM: ret.sym = SDLK1_KP_EXCLAM; break;
		//case SDLK_KP_MEMSTORE: ret.sym = SDLK1_KP_MEMSTORE; break;
		//case SDLK_KP_MEMRECALL: ret.sym = SDLK1_KP_MEMRECALL; break;
		//case SDLK_KP_MEMCLEAR: ret.sym = SDLK1_KP_MEMCLEAR; break;
		//case SDLK_KP_MEMADD: ret.sym = SDLK1_KP_MEMADD; break;
		//case SDLK_KP_MEMSUBTRACT: ret.sym = SDLK1_KP_MEMSUBTRACT; break;
		//case SDLK_KP_MEMMULTIPLY: ret.sym = SDLK1_KP_MEMMULTIPLY; break;
		//case SDLK_KP_MEMDIVIDE: ret.sym = SDLK1_KP_MEMDIVIDE; break;
		//case SDLK_KP_PLUSMINUS: ret.sym = SDLK1_KP_PLUSMINUS; break;
		//case SDLK_KP_CLEAR: ret.sym = SDLK1_KP_CLEAR; break;
		//case SDLK_KP_CLEARENTRY: ret.sym = SDLK1_KP_CLEARENTRY; break;
		//case SDLK_KP_BINARY: ret.sym = SDLK1_KP_BINARY; break;
		//case SDLK_KP_OCTAL: ret.sym = SDLK1_KP_OCTAL; break;
		//case SDLK_KP_DECIMAL: ret.sym = SDLK1_KP_DECIMAL; break;
		//case SDLK_KP_HEXADECIMAL: ret.sym = SDLK1_KP_HEXADECIMAL; break;
		case SDLK_LCTRL: ret.sym = SDLK1_LCTRL; break;
		case SDLK_LSHIFT: ret.sym = SDLK1_LSHIFT; break;
		case SDLK_LALT: ret.sym = SDLK1_LALT; break;
		//case SDLK_LGUI: ret.sym = SDLK1_LGUI; break;
		case SDLK_RCTRL: ret.sym = SDLK1_RCTRL; break;
		case SDLK_RSHIFT: ret.sym = SDLK1_RSHIFT; break;
		case SDLK_RALT: ret.sym = SDLK1_RALT; break;
		//case SDLK_RGUI: ret.sym = SDLK1_RGUI; break;
		case SDLK_MODE: ret.sym = SDLK1_MODE; break;
		//case SDLK_AUDIONEXT: ret.sym = SDLK1_AUDIONEXT; break;
		//case SDLK_AUDIOPREV: ret.sym = SDLK1_AUDIOPREV; break;
		//case SDLK_AUDIOSTOP: ret.sym = SDLK1_AUDIOSTOP; break;
		//case SDLK_AUDIOPLAY: ret.sym = SDLK1_AUDIOPLAY; break;
		//case SDLK_AUDIOMUTE: ret.sym = SDLK1_AUDIOMUTE; break;
		//case SDLK_MEDIASELECT: ret.sym = SDLK1_MEDIASELECT; break;
		//case SDLK_WWW: ret.sym = SDLK1_WWW; break;
		//case SDLK_MAIL: ret.sym = SDLK1_MAIL; break;
		//case SDLK_CALCULATOR: ret.sym = SDLK1_CALCULATOR; break;
		//case SDLK_COMPUTER: ret.sym = SDLK1_COMPUTER; break;
		//case SDLK_AC_SEARCH: ret.sym = SDLK1_AC_SEARCH; break;
		//case SDLK_AC_HOME: ret.sym = SDLK1_AC_HOME; break;
		//case SDLK_AC_BACK: ret.sym = SDLK1_AC_BACK; break;
		//case SDLK_AC_FORWARD: ret.sym = SDLK1_AC_FORWARD; break;
		//case SDLK_AC_STOP: ret.sym = SDLK1_AC_STOP; break;
		//case SDLK_AC_REFRESH: ret.sym = SDLK1_AC_REFRESH; break;
		//case SDLK_AC_BOOKMARKS: ret.sym = SDLK1_AC_BOOKMARKS; break;
		//case SDLK_BRIGHTNESSDOWN: ret.sym = SDLK1_BRIGHTNESSDOWN; break;
		//case SDLK_BRIGHTNESSUP: ret.sym = SDLK1_BRIGHTNESSUP; break;
		//case SDLK_DISPLAYSWITCH: ret.sym = SDLK1_DISPLAYSWITCH; break;
		//case SDLK_KBDILLUMTOGGLE: ret.sym = SDLK1_KBILLUMTOGGLE; break;
		//case SDLK_KBDILLUMDOWN: ret.sym = SDLK1_KBDILLUMDOWN; break;
		//case SDLK_KBDILLUMUP: ret.sym = SDLK1_KBDILLUMUP; break;
		//case SDLK_EJECT: ret.sym = SDLK1_EJECT; break;
		//case SDLK_SLEEP: ret.sym = SDLK1_SLEEP; break;
	}
	return ret;
}

void SDLCALL SDLCL_ProcessKeyEvent (SDL_Event *event2) {
	SDL1_Event event;
	if (!key_delay && event2->key.repeat) return;
	event.key.type = (event2->type == SDL_KEYDOWN) ? SDL1_KEYDOWN : SDL1_KEYUP;
	event.key.which = 0;
	event.key.state = (event2->key.state == SDL_PRESSED) ? SDL1_PRESSED : SDL1_RELEASED;
	event.key.keysym = keysym2to1(event2->key.keysym);
	key_state[event.key.keysym.sym] = event.key.state;
	SDLCL_ProcessEvent(&event);
	if (event2->type == SDL_KEYDOWN) {
		switch (event2->key.keysym.sym) {
			case SDLK_BACKSPACE: SDLCL_PushUnicode(0x08); break;
			case SDLK_TAB: SDLCL_PushUnicode(0x09); break;
			case SDLK_RETURN: SDLCL_PushUnicode(0x0D); break;
			case SDLK_ESCAPE: SDLCL_PushUnicode(0x1B); break;
			case SDLK_DELETE: SDLCL_PushUnicode(0x7F); break;
			default: break;
		}
	}
}
