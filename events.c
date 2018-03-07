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
#include "events.h"

static int unicode = 0;

DECLSPEC int SDLCALL SDL_EnableUNICODE (int enable) {
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

DECLSPEC int SDLCALL SDL_JoystickEventState (int state) {
	return rSDL_JoystickEventState(state);
}

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

DECLSPEC int SDLCALL SDL_PeepEvents (SDL1_Event *events, int numevents, SDL1_eventaction action, Uint32 mask) {
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

DECLSPEC int SDLCALL SDL_PushEvent(SDL1_Event *event) {
	if (SDL_PeepEvents(event, 1, SDL1_ADDEVENT, 0) <= 0) return -1;
	return 0;
}

typedef int (SDLCALL *SDL1_EventFilter)(const SDL1_Event *event);
static SDL1_EventFilter event_filter = NULL;

DECLSPEC void SDLCALL SDL_SetEventFilter (SDL1_EventFilter filter) {
	event_filter = filter;
}

DECLSPEC SDL1_EventFilter SDLCALL SDL_GetEventFilter (void) {
	return event_filter;
}

#define MAXUNICODE 128
static struct {
	int head;
	int tail;
	Uint16 unicode[MAXUNICODE];
} unicode_queue = { 0, 0 , { 0 }};

static SDL1_Event unicode_event = { SDL1_NOEVENT };

static Uint8 event_state[SDL1_NUM_EVENTS] = {
	SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE,
	SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE,
	SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE,
	SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE,
	SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE,
	SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE,
	SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE,
	SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE, SDL1_ENABLE
};

DECLSPEC Uint8 SDL_EventState (Uint8 type, int state) {
	int ret;
	if (type >= SDL1_NUM_EVENTS) return SDL1_IGNORE;
	ret = event_state[type];
	if (state != SDL_QUERY)
		event_state[type] = state;
	return ret;
}

static void add_event_filtered (SDL1_Event *event) {
	if (event_state[event->type] && (!event_filter || event_filter(event))) SDL_PushEvent(event);
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

void SDLCL_ProcessEvent (SDL1_Event *event) {
	flush_unicode();
	if (event->type == SDL1_KEYDOWN) {
		if (pull_unicode(event)) add_event_filtered(event);
		else unicode_event = *event;
	} else {
		add_event_filtered(event);
	}
}

DECLSPEC void SDLCALL SDL_PumpEvents (void) {
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
						SDLCL_ProcessEvent(&event);
						break;
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
						event.active.gain = 1;
						event.active.state = SDL1_APPACTIVE;
						update_active(&event.active);
						SDLCL_ProcessEvent(&event);
						break;
					case SDL_WINDOWEVENT_ENTER:
						event.active.gain = 1;
						event.active.state = SDL1_APPMOUSEFOCUS;
						update_active(&event.active);
						SDLCL_ProcessEvent(&event);
						break;
					case SDL_WINDOWEVENT_LEAVE:
						event.active.gain = 0;
						event.active.state = SDL1_APPMOUSEFOCUS;
						update_active(&event.active);
						SDLCL_ProcessEvent(&event);
						break;
					case SDL_WINDOWEVENT_FOCUS_GAINED:
						event.active.gain = 1;
						event.active.state = SDL1_APPINPUTFOCUS;
						update_active(&event.active);
						SDLCL_ProcessEvent(&event);
						break;
					case SDL_WINDOWEVENT_FOCUS_LOST:
						event.active.gain = 0;
						event.active.state = SDL1_APPINPUTFOCUS;
						update_active(&event.active);
						SDLCL_ProcessEvent(&event);
						break;
					default:
						break;
				}
				break;
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				SDLCL_ProcessKeyEvent(&event2);
				break;
			case SDL_TEXTINPUT:
				push_text(event2.text.text);
				break;
			case SDL_MOUSEMOTION:
				SDLCL_ProcessMouseMotion(&event2);
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				SDLCL_ProcessMouseButton(&event2);
				break;
			case SDL_MOUSEWHEEL:
				SDLCL_ProcessMouseWheel(&event2);
				break;
			case SDL_QUIT:
				event.quit.type = SDL1_QUIT;
				SDLCL_ProcessEvent(&event);
				break;
			default:
				break;
		}
	}
	flush_unicode();
}

DECLSPEC int SDLCALL SDL_PollEvent (SDL1_Event *event) {
	SDL_PumpEvents();
	if (SDL_PeepEvents(event, 1, SDL1_GETEVENT, SDL1_ALLEVENTS) <= 0) return 0;
	return 1;
}

DECLSPEC int SDLCALL SDL_WaitEvent (SDL1_Event *event) {
	while (!SDL_PollEvent(event)) {
		if (!rSDL_WaitEvent(NULL)) return 0;
	}
	return 1;
}

DECLSPEC Uint8 SDLCALL SDL_GetAppState (void) {
	return active_map;
}
