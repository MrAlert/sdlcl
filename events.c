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

static int unicode = 0;

int SDLCALL SDL_EnableUNICODE (int enable) {
	int oldunicode = unicode;
	switch (enable) {
		case 0:
		case 1:
			unicode = enable;
		default:
			break;
	}
	if (unicode != oldunicode) {
		if (unicode) rSDL_StartTextInput();
		else rSDL_StopTextInput();
	}
	return oldunicode;
}

static int key_delay = 1;
static int key_interval = 1;

int SDLCALL SDL_EnableKeyRepeat (int delay, int interval) {
	key_delay = delay;
	key_interval = interval;
	return 0;
}

int SDLCALL SDL_JoystickEventState (int state) {
	return rSDL_JoystickEventState(state);
}

typedef enum {
	KMOD1_NONE     = 0x0000,
	KMOD1_LSHIFT   = 0x0001,
	KMOD1_RSHIFT   = 0x0002,
	KMOD1_LCTRL    = 0x0040,
	KMOD1_RCTRL    = 0x0080,
	KMOD1_LALT     = 0x0100,
	KMOD1_RALT     = 0x0200,
	KMOD1_LMETA    = 0x0400,
	KMOD1_RMETA    = 0x0800,
	KMOD1_NUM      = 0x1000,
	KMOD1_CAPS     = 0x2000,
	KMOD1_MODE     = 0x4000,
	KMOD1_RESERVED = 0x8000
} SDL1Mod;

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

SDL1Mod SDLCALL SDL_GetModState (void) {
	return keymod2to1(rSDL_GetModState());
}

typedef enum {
	SDLK1_UNKNOWN      = 0,
	SDLK1_FIRST        = 0,
	SDLK1_BACKSPACE    = 8,
	SDLK1_TAB          = 9,
	SDLK1_CLEAR        = 12,
	SDLK1_RETURN       = 13,
	SDLK1_PAUSE        = 19,
	SDLK1_ESCAPE       = 27,
	SDLK1_SPACE        = 32,
	SDLK1_EXCLAIM      = 33,
	SDLK1_QUOTEDBL     = 34,
	SDLK1_HASH         = 35,
	SDLK1_DOLLAR       = 36,
	SDLK1_AMPERSAND    = 38,
	SDLK1_QUOTE        = 39,
	SDLK1_LEFTPAREN    = 40,
	SDLK1_RIGHTPAREN   = 41,
	SDLK1_ASTERISK     = 42,
	SDLK1_PLUS         = 43,
	SDLK1_COMMA        = 44,
	SDLK1_MINUS        = 45,
	SDLK1_PERIOD       = 46,
	SDLK1_SLASH        = 47,
	SDLK1_0            = 48,
	SDLK1_1            = 49,
	SDLK1_2            = 50,
	SDLK1_3            = 51,
	SDLK1_4            = 52,
	SDLK1_5            = 53,
	SDLK1_6            = 54,
	SDLK1_7            = 55,
	SDLK1_8            = 56,
	SDLK1_9            = 57,
	SDLK1_COLON        = 58,
	SDLK1_SEMICOLON    = 59,
	SDLK1_LESS         = 60,
	SDLK1_EQUALS       = 61,
	SDLK1_GREATER      = 62,
	SDLK1_QUESTION     = 63,
	SDLK1_AT           = 64,
	SDLK1_LEFTBRACKET  = 91,
	SDLK1_BACKSLASH    = 92,
	SDLK1_RIGHTBRACKET = 93,
	SDLK1_CARET        = 94,
	SDLK1_UNDERSCORE   = 95,
	SDLK1_BACKQUOTE    = 96,
	SDLK1_a            = 97,
	SDLK1_b            = 98,
	SDLK1_c            = 99,
	SDLK1_d            = 100,
	SDLK1_e            = 101,
	SDLK1_f            = 102,
	SDLK1_g            = 103,
	SDLK1_h            = 104,
	SDLK1_i            = 105,
	SDLK1_j            = 106,
	SDLK1_k            = 107,
	SDLK1_l            = 108,
	SDLK1_m            = 109,
	SDLK1_n            = 110,
	SDLK1_o            = 111,
	SDLK1_p            = 112,
	SDLK1_q            = 113,
	SDLK1_r            = 114,
	SDLK1_s            = 115,
	SDLK1_t            = 116,
	SDLK1_u            = 117,
	SDLK1_v            = 118,
	SDLK1_w            = 119,
	SDLK1_x            = 120,
	SDLK1_y            = 121,
	SDLK1_z            = 122,
	SDLK1_DELETE       = 127,
	SDLK1_WORLD_0      = 160,
	SDLK1_WORLD_1      = 161,
	SDLK1_WORLD_2      = 162,
	SDLK1_WORLD_3      = 163,
	SDLK1_WORLD_4      = 164,
	SDLK1_WORLD_5      = 165,
	SDLK1_WORLD_6      = 166,
	SDLK1_WORLD_7      = 167,
	SDLK1_WORLD_8      = 168,
	SDLK1_WORLD_9      = 169,
	SDLK1_WORLD_10     = 170,
	SDLK1_WORLD_11     = 171,
	SDLK1_WORLD_12     = 172,
	SDLK1_WORLD_13     = 173,
	SDLK1_WORLD_14     = 174,
	SDLK1_WORLD_15     = 175,
	SDLK1_WORLD_16     = 176,
	SDLK1_WORLD_17     = 177,
	SDLK1_WORLD_18     = 178,
	SDLK1_WORLD_19     = 179,
	SDLK1_WORLD_20     = 180,
	SDLK1_WORLD_21     = 181,
	SDLK1_WORLD_22     = 182,
	SDLK1_WORLD_23     = 183,
	SDLK1_WORLD_24     = 184,
	SDLK1_WORLD_25     = 185,
	SDLK1_WORLD_26     = 186,
	SDLK1_WORLD_27     = 187,
	SDLK1_WORLD_28     = 188,
	SDLK1_WORLD_29     = 189,
	SDLK1_WORLD_30     = 190,
	SDLK1_WORLD_31     = 191,
	SDLK1_WORLD_32     = 192,
	SDLK1_WORLD_33     = 193,
	SDLK1_WORLD_34     = 194,
	SDLK1_WORLD_35     = 195,
	SDLK1_WORLD_36     = 196,
	SDLK1_WORLD_37     = 197,
	SDLK1_WORLD_38     = 198,
	SDLK1_WORLD_39     = 199,
	SDLK1_WORLD_40     = 200,
	SDLK1_WORLD_41     = 201,
	SDLK1_WORLD_42     = 202,
	SDLK1_WORLD_43     = 203,
	SDLK1_WORLD_44     = 204,
	SDLK1_WORLD_45     = 205,
	SDLK1_WORLD_46     = 206,
	SDLK1_WORLD_47     = 207,
	SDLK1_WORLD_48     = 208,
	SDLK1_WORLD_49     = 209,
	SDLK1_WORLD_50     = 210,
	SDLK1_WORLD_51     = 211,
	SDLK1_WORLD_52     = 212,
	SDLK1_WORLD_53     = 213,
	SDLK1_WORLD_54     = 214,
	SDLK1_WORLD_55     = 215,
	SDLK1_WORLD_56     = 216,
	SDLK1_WORLD_57     = 217,
	SDLK1_WORLD_58     = 218,
	SDLK1_WORLD_59     = 219,
	SDLK1_WORLD_60     = 220,
	SDLK1_WORLD_61     = 221,
	SDLK1_WORLD_62     = 222,
	SDLK1_WORLD_63     = 223,
	SDLK1_WORLD_64     = 224,
	SDLK1_WORLD_65     = 225,
	SDLK1_WORLD_66     = 226,
	SDLK1_WORLD_67     = 227,
	SDLK1_WORLD_68     = 228,
	SDLK1_WORLD_69     = 229,
	SDLK1_WORLD_70     = 230,
	SDLK1_WORLD_71     = 231,
	SDLK1_WORLD_72     = 232,
	SDLK1_WORLD_73     = 233,
	SDLK1_WORLD_74     = 234,
	SDLK1_WORLD_75     = 235,
	SDLK1_WORLD_76     = 236,
	SDLK1_WORLD_77     = 237,
	SDLK1_WORLD_78     = 238,
	SDLK1_WORLD_79     = 239,
	SDLK1_WORLD_80     = 240,
	SDLK1_WORLD_81     = 241,
	SDLK1_WORLD_82     = 242,
	SDLK1_WORLD_83     = 243,
	SDLK1_WORLD_84     = 244,
	SDLK1_WORLD_85     = 245,
	SDLK1_WORLD_86     = 246,
	SDLK1_WORLD_87     = 247,
	SDLK1_WORLD_88     = 248,
	SDLK1_WORLD_89     = 249,
	SDLK1_WORLD_90     = 250,
	SDLK1_WORLD_91     = 251,
	SDLK1_WORLD_92     = 252,
	SDLK1_WORLD_93     = 253,
	SDLK1_WORLD_94     = 254,
	SDLK1_WORLD_95     = 255,
	SDLK1_KP0          = 256,
	SDLK1_KP1          = 257,
	SDLK1_KP2          = 258,
	SDLK1_KP3          = 259,
	SDLK1_KP4          = 260,
	SDLK1_KP5          = 261,
	SDLK1_KP6          = 262,
	SDLK1_KP7          = 263,
	SDLK1_KP8          = 264,
	SDLK1_KP9          = 265,
	SDLK1_KP_PERIOD    = 266,
	SDLK1_KP_DIVIDE    = 267,
	SDLK1_KP_MULTIPLY  = 268,
	SDLK1_KP_MINUS     = 269,
	SDLK1_KP_PLUS      = 270,
	SDLK1_KP_ENTER     = 271,
	SDLK1_KP_EQUALS    = 272,
	SDLK1_UP           = 273,
	SDLK1_DOWN         = 274,
	SDLK1_RIGHT        = 275,
	SDLK1_LEFT         = 276,
	SDLK1_INSERT       = 277,
	SDLK1_HOME         = 278,
	SDLK1_END          = 279,
	SDLK1_PAGEUP       = 280,
	SDLK1_PAGEDOWN     = 281,
	SDLK1_F1           = 282,
	SDLK1_F2           = 283,
	SDLK1_F3           = 284,
	SDLK1_F4           = 285,
	SDLK1_F5           = 286,
	SDLK1_F6           = 287,
	SDLK1_F7           = 288,
	SDLK1_F8           = 289,
	SDLK1_F9           = 290,
	SDLK1_F10          = 291,
	SDLK1_F11          = 292,
	SDLK1_F12          = 293,
	SDLK1_F13          = 294,
	SDLK1_F14          = 295,
	SDLK1_F15          = 296,
	SDLK1_NUMLOCK      = 300,
	SDLK1_CAPSLOCK     = 301,
	SDLK1_SCROLLOCK    = 302,
	SDLK1_RSHIFT       = 303,
	SDLK1_LSHIFT       = 304,
	SDLK1_RCTRL        = 305,
	SDLK1_LCTRL        = 306,
	SDLK1_RALT         = 307,
	SDLK1_LALT         = 308,
	SDLK1_RMETA        = 309,
	SDLK1_LMETA        = 310,
	SDLK1_LSUPER       = 311,
	SDLK1_RSUPER       = 312,
	SDLK1_MODE         = 313,
	SDLK1_COMPOSE      = 314,
	SDLK1_HELP         = 315,
	SDLK1_PRINT        = 316,
	SDLK1_SYSREQ       = 317,
	SDLK1_BREAK        = 318,
	SDLK1_MENU         = 319,
	SDLK1_POWER        = 320,
	SDLK1_EURO         = 321,
	SDLK1_UNDO         = 322,
	SDLK1_LAST
} SDL1Key;

static Uint8 key_state[SDLK1_LAST] = { 0 };

Uint8 *SDLCALL SDL_GetKeyState (int *numkeys) {
	if (numkeys) *numkeys = SDLK1_LAST;
	return key_state;
}

typedef struct SDL1_keysym {
	Uint8 scancode;
	SDL1Key sym;
	SDL1Mod mod;
	Uint16 unicode;
} SDL1_keysym;

static SDL1_keysym keysym2to1 (SDL_Keysym keysym) {
	SDL1_keysym ret;
	/* scancode is supposed to be hardware-dependent, so just put this here */
	ret.scancode = keysym.scancode;
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
		//case SDLK_KP_1: ret.sym = SDLK1_KP_1; break;
		//case SDLK_KP_2: ret.sym = SDLK1_KP_2; break;
		//case SDLK_KP_3: ret.sym = SDLK1_KP_3; break;
		//case SDLK_KP_4: ret.sym = SDLK1_KP_4; break;
		//case SDLK_KP_5: ret.sym = SDLK1_KP_5; break;
		//case SDLK_KP_6: ret.sym = SDLK1_KP_6; break;
		//case SDLK_KP_7: ret.sym = SDLK1_KP_7; break;
		//case SDLK_KP_8: ret.sym = SDLK1_KP_8; break;
		//case SDLK_KP_9: ret.sym = SDLK1_KP_9; break;
		//case SDLK_KP_0: ret.sym = SDLK1_KP_0; break;
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

typedef enum {
	SDL1_NOEVENT = 0,
	SDL1_ACTIVEEVENT,
	SDL1_KEYDOWN,
	SDL1_KEYUP,
	SDL1_MOUSEMOTION,
	SDL1_MOUSEBUTTONDOWN,
	SDL1_MOUSEBUTTONUP,
	SDL1_JOYAXISMOTION,
	SDL1_JOYBALLMOTION,
	SDL1_JOYHATMOTION,
	SDL1_JOYBUTTONDOWN,
	SDL1_JOYBUTTONUP,
	SDL1_QUIT,
	SDL1_SYSWMEVENT,
	SDL1_EVENT_RESERVEDA,
	SDL1_EVENT_RESERVEDB,
	SDL1_VIDEORESIZE,
	SDL1_VIDEOEXPOSE,
	SDL1_EVENT_RESERVED2,
	SDL1_EVENT_RESERVED3,
	SDL1_EVENT_RESERVED4,
	SDL1_EVENT_RESERVED5,
	SDL1_EVENT_RESERVED6,
	SDL1_EVENT_RESERVED7,
	SDL1_USEREVENT = 24,
	SDL1_NUM_EVENTS = 32
} SDL1_EventType;

#define SDL1_EVENTMASK(X) (1<<(X))
#define SDL1_ALLEVENTS 0xFFFFFFFF

typedef struct SDL1_ActiveEvent {
	Uint8 type;
	Uint8 gain;
	Uint8 state;
} SDL1_ActiveEvent;

typedef struct SDL1_KeyboardEvent {
	Uint8 type;
	Uint8 which;
	Uint8 state;
	SDL1_keysym keysym;
} SDL1_KeyboardEvent;

typedef struct SDL1_MouseMotionEvent {
	Uint8 type;
	Uint8 which;
	Uint8 state;
	Uint16 x, y;
	Sint16 xrel;
	Sint16 yrel;
} SDL1_MouseMotionEvent;

typedef struct SDL1_MouseButtonEvent {
	Uint8 type;
	Uint8 which;
	Uint8 button;
	Uint8 state;
	Uint16 x, y;
} SDL1_MouseButtonEvent;

typedef struct SDL1_QuitEvent {
	Uint8 type;
} SDL1_QuitEvent;

typedef union SDL1_Event {
	Uint8 type;
	SDL1_ActiveEvent active;
	SDL1_KeyboardEvent key;
	SDL1_MouseMotionEvent motion;
	SDL1_MouseButtonEvent button;
	//SDL1_JoyAxisEvent jaxis;
	//SDL1_JoyBallEvent jball;
	//SDL1_JoyHatEvent jhat;
	//SDL1_JoyButtonEvent jbutton;
	//SDL1_ResizeEvent resize;
	//SDL1_ExposeEvent expose;
	SDL1_QuitEvent quit;
	//SDL1_UserEvent user;
	//SDL1_SysWMEvent syswm;
} SDL1_Event;

#define SDL1_RELEASED 0
#define SDL1_PRESSED  1

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

Uint8 SDLCALL SDL_GetMouseState (int *x, int *y) {
	return mousestate2to1(rSDL_GetMouseState(x, y));
}

Uint8 SDLCALL SDL_GetRelativeMouseState (int *x, int *y) {
	return mousestate2to1(rSDL_GetRelativeMouseState(x, y));
}

static Uint16 lastx = 0;
static Uint16 lasty = 0;

#define SDL1_APPMOUSEFOCUS 0x01
#define SDL1_APPINPUTFOCUS 0x02
#define SDL1_APPACTIVE     0x04

static Uint8 active_map = 0;

static void update_active (SDL1_ActiveEvent *active) {
	if (active->gain) active_map |= active->state;
	else active_map &= ~active->state;
}


#define MAXEVENTS 128
static struct {
	SDL_mutex *lock;
	int head;
	int tail;
	SDL1_Event event[MAXEVENTS];
} event_queue = { NULL, 0, 0 , { { SDL1_NOEVENT } }};

static int add_event (SDL1_Event *event) {
	int tail = (event_queue.tail + 1) % MAXEVENTS;
	if (tail != event_queue.head) {
		event_queue.event[event_queue.tail] = *event;
		event_queue.tail = tail;
		return 1;
	} else {
		return 0;
	}
}

static int cut_event (int i) {
	int here, next;
	if (i == event_queue.head) {
		event_queue.head = (event_queue.head + 1) % MAXEVENTS;
		return event_queue.head;
	} else if ((i + 1) % MAXEVENTS == event_queue.tail) {
		event_queue.tail = i;
		return event_queue.tail;
	} else {
		if (--event_queue.tail < 0) event_queue.tail = MAXEVENTS - 1;
		for (here = i; here != event_queue.tail; here = next) {
			next = (here + 1) % MAXEVENTS;
			event_queue.event[here] = event_queue.event[next];
		}
		return i;
	}
}

typedef enum {
	SDL1_ADDEVENT,
	SDL1_PEEKEVENT,
	SDL1_GETEVENT
} SDL1_eventaction;

int SDLCALL SDL_PeepEvents (SDL1_Event *events, int numevents, SDL1_eventaction action, Uint32 mask) {
	SDL1_Event tmpevent;
	int i, used = 0;
	if (event_queue.lock && !rSDL_LockMutex(event_queue.lock)) {
		if (action == SDL1_ADDEVENT) {
			for (i = 0; i < numevents; i++)
				used += add_event(&events[i]);
		} else {
			if (!events) {
				action = SDL1_PEEKEVENT;
				numevents = 1;
				events = &tmpevent;
			}
			i = event_queue.head;
			while ((used < numevents) && (i != event_queue.tail)) {
				if (mask & SDL1_EVENTMASK(event_queue.event[i].type)) {
					events[used++] = event_queue.event[i];
					if (action == SDL1_GETEVENT) i = cut_event(i);
					else i = (i + 1) % MAXEVENTS;
				} else {
					i = (i + 1) % MAXEVENTS;
				}
			}
		}
		rSDL_UnlockMutex(event_queue.lock);
		return used;
	} else {
		return -1;
	}
}

int SDLCALL SDL_PushEvent(SDL1_Event *event) {
	if (SDL_PeepEvents(event, 1, SDL1_ADDEVENT, 0) <= 0) return -1;
	return 0;
}

typedef int (SDLCALL *SDL1_EventFilter)(const SDL1_Event *event);
static SDL1_EventFilter event_filter = NULL;

void SDLCALL SDL_SetEventFilter (SDL1_EventFilter filter) {
	event_filter = filter;
}

SDL1_EventFilter SDLCALL SDL_GetEventFilter (void) {
	return event_filter;
}

#define MAXUNICODE 128
static struct {
	int head;
	int tail;
	Uint16 unicode[MAXUNICODE];
} unicode_queue = { 0, 0 , { 0 }};

static SDL1_Event unicode_event = { SDL1_NOEVENT };

static void add_event_filtered (SDL1_Event *event) {
	if (!event_filter || event_filter(event)) SDL_PushEvent(event);
}

/* Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de> */
/* See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details. */

#define UTF8_ACCEPT 0
#define UTF8_REJECT 12

static const Uint8 utf8d[] = {
	/* The first part of the table maps bytes to character classes that */
	/* to reduce the size of the transition table and create bitmasks. */
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
	 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	 8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,
	/* The second part is a transition table that maps a combination */
	/* of a state of the automaton and a character class to a state. */
	 0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
	12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
	12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
	12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
	12,36,12,12,12,12,12,12,12,12,12,12, 
};

static Uint32 utf8_decode (Uint32 *state, Uint32 *codep, Uint32 byte) {
	Uint32 type = utf8d[byte];
	*codep = (*state != UTF8_ACCEPT) ?
		(byte & 0x3fu) | (*codep << 6) :
		(0xff >> type) & (byte);
	*state = utf8d[256 + *state + type];
	return *state;
}

static void push_unicode (Uint16 unicode) {
	int tail;
	tail = (unicode_queue.tail + 1) % MAXUNICODE;
	if (tail == unicode_queue.head) return;
	unicode_queue.unicode[unicode_queue.tail] = unicode;
	unicode_queue.tail = tail;
}

static void push_text (const char *text) {
	Uint32 state = 0;
	Uint32 codepoint;
	while (*text) {
		if (utf8_decode(&state, &codepoint, *((Uint8 *)(text++)))) continue;
		if (codepoint > 0xFFFF) {
			push_unicode(0xD7C0 + (codepoint >> 10));
			push_unicode(0xDC00 + (codepoint & 0x3FF));
		} else {
			push_unicode(codepoint);
		}
	}
}

static int pull_unicode (SDL1_Event *event) {
	if (unicode_queue.head != unicode_queue.tail) {
		event->key.keysym.unicode = unicode_queue.unicode[unicode_queue.head];
		unicode_queue.head = (unicode_queue.head + 1) % MAXUNICODE;
		return 1;
	} else {
		return 0;
	}
}

static void flush_unicode (void) {
	SDL1_Event event;
	if (unicode_event.type != SDL1_NOEVENT) {
		pull_unicode(&unicode_event);
		add_event_filtered(&unicode_event);
		unicode_event.type = SDL1_NOEVENT;
	}
	event.key.type = SDL1_KEYDOWN;
	event.key.which = 0;
	event.key.state = SDL1_PRESSED;
	event.key.keysym.scancode = 0;
	event.key.keysym.sym = SDLK1_UNKNOWN;
	event.key.keysym.mod = KMOD1_NONE;
	while (unicode_queue.head != unicode_queue.tail) {
		event.key.keysym.unicode = unicode_queue.unicode[unicode_queue.head];
		unicode_queue.head = (unicode_queue.head + 1) % MAXUNICODE;
		add_event_filtered(&event);
	}
}

static void process_event (SDL1_Event *event) {
	flush_unicode();
	if (event->type == SDL1_KEYDOWN) {
		if (pull_unicode(event)) add_event_filtered(event);
		else unicode_event = *event;
	} else {
		add_event_filtered(event);
	}
}

void SDLCALL SDL_PumpEvents (void) {
	SDL1_Event event;
	SDL_Event event2;
	if (!event_queue.lock) {
		/* SDL_PumpEvents() is only supposed to be called from one thread, */
		/* so initializing like this should be safe. */
		event_queue.lock = rSDL_CreateMutex();
	}
	while (rSDL_PollEvent(&event2)) {
		switch (event2.type) {
			case SDL_WINDOWEVENT:
				event.active.type = SDL1_ACTIVEEVENT;
				switch (event2.window.event) {
					case SDL_WINDOWEVENT_MINIMIZED:
						event.active.gain = 0;
						event.active.state = SDL1_APPACTIVE;
						update_active(&event.active);
						process_event(&event);
						break;
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
						event.active.gain = 1;
						event.active.state = SDL1_APPACTIVE;
						update_active(&event.active);
						process_event(&event);
						break;
					case SDL_WINDOWEVENT_ENTER:
						event.active.gain = 1;
						event.active.state = SDL1_APPMOUSEFOCUS;
						update_active(&event.active);
						process_event(&event);
						break;
					case SDL_WINDOWEVENT_LEAVE:
						event.active.gain = 0;
						event.active.state = SDL1_APPMOUSEFOCUS;
						update_active(&event.active);
						process_event(&event);
						break;
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						event.active.gain = 1;
						event.active.state = SDL1_APPINPUTFOCUS;
						update_active(&event.active);
						process_event(&event);
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						event.active.gain = 0;
						event.active.state = SDL1_APPINPUTFOCUS;
						update_active(&event.active);
						process_event(&event);
						break;
					default:
						break;
				}
				break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				if (!key_delay && event2.key.repeat) break;
				event.key.type = (event2.type == SDL_KEYDOWN) ? SDL1_KEYDOWN : SDL1_KEYUP;
				event.key.which = 0;
				event.key.state = event2.key.state;
				event.key.keysym = keysym2to1(event2.key.keysym);
				key_state[event.key.keysym.sym] = event.key.state;
				process_event(&event);
				break;
			case SDL_TEXTINPUT:
				push_text(event2.text.text);
				break;
			case SDL_MOUSEMOTION:
				event.motion.type = SDL1_MOUSEMOTION;
				event.motion.which = 0;
				event.motion.state = mousestate2to1(event2.motion.state);
				event.motion.x = event2.motion.x;
				event.motion.y = event2.motion.y;
				event.motion.xrel = event2.motion.xrel;
				event.motion.yrel = event2.motion.yrel;
				lastx = event2.motion.x;
				lasty = event2.motion.y;
				process_event(&event);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				event.button.type = (event2.type == SDL_MOUSEBUTTONDOWN) ? SDL1_MOUSEBUTTONDOWN : SDL1_MOUSEBUTTONUP;
				event.button.which = 0;
				event.button.button = mousebutton2to1(event2.button.button);
				event.button.state = (event2.button.state == SDL_PRESSED) ? SDL1_PRESSED : SDL1_RELEASED;
				event.button.x = event2.button.x;
				event.button.y = event2.button.y;
				lastx = event2.button.x;
				lasty = event2.button.y;
				process_event(&event);
				break;
			case SDL_MOUSEWHEEL:
				if (event2.wheel.y == 0) break;
				event.button.type = SDL1_MOUSEBUTTONDOWN;
				event.button.which = 0;
				event.button.button = (event2.wheel.y > 0) ? SDL1_BUTTON_WHEELUP : SDL1_BUTTON_WHEELDOWN;
				event.button.state = SDL1_PRESSED;
				event.button.x = lastx;
				event.button.y = lasty;
				process_event(&event);
				event.button = event.button;
				event.button.type = SDL1_MOUSEBUTTONUP;
				event.button.state = SDL1_RELEASED;
				process_event(&event);
				break;
			case SDL_QUIT:
				event.quit.type = SDL1_QUIT;
				process_event(&event);
				break;
			default:
				break;
		}
	}
	flush_unicode();
}

int SDLCALL SDL_PollEvent (SDL1_Event *event) {
	SDL_PumpEvents();
	if (SDL_PeepEvents(event, 1, SDL1_GETEVENT, SDL1_ALLEVENTS) <= 0) return 0;
	return 1;
}

int SDLCALL SDL_WaitEvent (SDL1_Event *event) {
	while (!SDL_PollEvent(event)) {
		if (!rSDL_WaitEvent(NULL)) return 0;
	}
	return 1;
}

Uint8 SDLCALL SDL_GetAppState (void) {
	return active_map;
}
