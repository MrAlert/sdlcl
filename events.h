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

#ifndef SDLCL_EVENTS_H
#define SDLCL_EVENTS_H

#include "SDL2.h"

#define SDL1_DISABLE 0
#define SDL1_ENABLE  1
#define SDL1_QUERY   -1

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

typedef struct SDL1_keysym {
	Uint8 scancode;
	SDL1Key sym;
	SDL1Mod mod;
	Uint16 unicode;
} SDL1_keysym;

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

/* General event functions */
extern void SDLCALL SDLCL_ProcessEvent (SDL1_Event *event);

/* Keyboard event functions */
extern void SDLCALL SDLCL_ProcessKeyEvent (SDL_Event *event2);

/* Mouse event functions */
extern DECLSPEC Uint8 SDLCALL SDL_GetMouseState (int *x, int *y);
extern void SDLCALL SDLCL_SetMouseRange (int x, int y);
extern void SDLCALL SDLCL_GetMouseOffset (int *x, int *y);
extern void SDLCALL SDLCL_ProcessMouseMotion (SDL_Event *event2);
extern void SDLCALL SDLCL_ProcessMouseButton (SDL_Event *event2);
extern void SDLCALL SDLCL_ProcessMouseWheel (SDL_Event *event2);

#endif
