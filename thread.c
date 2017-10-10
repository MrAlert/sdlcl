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

DECLSPEC SDL_Thread *SDLCALL SDL_CreateThread (int (*fn)(void *), void *data) {
	return rSDL_CreateThread(fn, NULL, data);
}

DECLSPEC void SDLCALL SDL_WaitThread (SDL_Thread *thread, int *status) {
	rSDL_WaitThread(thread, status);
}

DECLSPEC void SDLCALL SDL_KillThread (SDL_Thread *thread) {
	(void)thread;
	/* Kill this thread? How about all threads!? */
	abort();
}

/* TODO: implement a way to work around 64-bit SDL_ThreadID */
DECLSPEC Uint32 SDLCALL SDL_ThreadID (void) {
	return (Uint32)rSDL_ThreadID();
}

DECLSPEC Uint32 SDLCALL SDL_GetThreadID (SDL_Thread *thread) {
	return (Uint32)rSDL_GetThreadID(thread);
}

/* Undefine SDL 2.0 mutex macros */
#undef SDL_mutexP
#undef SDL_mutexV

DECLSPEC SDL_mutex *SDLCALL SDL_CreateMutex (void) {
	return rSDL_CreateMutex();
}

DECLSPEC int SDLCALL SDL_mutexP (SDL_mutex *mutex) {
	return rSDL_LockMutex(mutex);
}

DECLSPEC int SDLCALL SDL_mutexV (SDL_mutex *mutex) {
	return rSDL_UnlockMutex(mutex);
}

DECLSPEC void SDLCALL SDL_DestroyMutex (SDL_mutex *mutex) {
	rSDL_DestroyMutex(mutex);
}

#define SDL1_MUTEX_TIMEDOUT 1

DECLSPEC SDL_sem *SDLCALL SDL_CreateSemaphore (Uint32 initial_value) {
	return rSDL_CreateSemaphore(initial_value);
}

DECLSPEC int SDLCALL SDL_SemWait (SDL_sem *sem) {
	return rSDL_SemWait(sem);
}

DECLSPEC int SDLCALL SDL_SemTryWait (SDL_sem *sem) {
	int ret = rSDL_SemTryWait(sem);
	if (ret == SDL_MUTEX_TIMEDOUT) return SDL1_MUTEX_TIMEDOUT;
	else return ret;
}

DECLSPEC int SDLCALL SDL_SemWaitTimeout (SDL_sem *sem, Uint32 timeout) {
	int ret = rSDL_SemWaitTimeout(sem, timeout);
	if (ret == SDL_MUTEX_TIMEDOUT) return SDL1_MUTEX_TIMEDOUT;
	else return ret;
}

DECLSPEC int SDLCALL SDL_SemPost (SDL_sem *sem) {
	return rSDL_SemPost(sem);
}

DECLSPEC Uint32 SDLCALL SDL_SemValue (SDL_sem *sem) {
	return rSDL_SemValue(sem);
}

DECLSPEC void SDLCALL SDL_DestroySemaphore (SDL_sem *sem) {
	rSDL_DestroySemaphore(sem);
}

DECLSPEC SDL_cond *SDLCALL SDL_CreateCond (void) {
	return rSDL_CreateCond();
}

DECLSPEC int SDLCALL SDL_CondSignal (SDL_cond *cond) {
	return rSDL_CondSignal(cond);
}

DECLSPEC int SDLCALL SDL_CondBroadcast (SDL_cond *cond) {
	return rSDL_CondBroadcast(cond);
}

DECLSPEC int SDLCALL SDL_CondWait (SDL_cond *cond, SDL_mutex *mut) {
	return rSDL_CondWait(cond, mut);
}

DECLSPEC int SDLCALL SDL_CondWaitTimeout (SDL_cond *cond, SDL_mutex *mutex, Uint32 ms) {
	int ret = rSDL_CondWaitTimeout(cond, mutex, ms);
	if (ret == SDL_MUTEX_TIMEDOUT) return SDL1_MUTEX_TIMEDOUT;
	else return ret;
}

DECLSPEC void SDLCALL SDL_DestroyCond (SDL_cond *cond) {
	rSDL_DestroyCond(cond);
}
