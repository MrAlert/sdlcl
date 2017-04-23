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

SDL2_SYMBOL(SDL_Init, int, (Uint32 flags))
SDL2_SYMBOL(SDL_InitSubSystem, int, (Uint32 flags))
SDL2_SYMBOL(SDL_GetError, const char *, (void))
SDL2_SYMBOL(SDL_QuitSubSystem, void, (Uint32 flags))
SDL2_SYMBOL(SDL_Quit, void, (void))
SDL2_SYMBOL(SDL_WasInit, Uint32, (Uint32 flags))

/* Timer subsystem */
SDL2_SYMBOL(SDL_GetTicks, Uint32, (void))
SDL2_SYMBOL(SDL_Delay, void, (Uint32 ms))

/* Video subsystem */
SDL2_SYMBOL(SDL_GetNumVideoDisplays, int, (void))
SDL2_SYMBOL(SDL_GetNumDisplayModes, int, (int displayIndex))
SDL2_SYMBOL(SDL_GetDisplayMode, int, (int displayIndex, int modeIndex, SDL_DisplayMode *mode))
SDL2_SYMBOL(SDL_GetDesktopDisplayMode, int, (int displayIndex, SDL_DisplayMode *mode))
SDL2_SYMBOL(SDL_GetPixelFormatName, const char *, (Uint32 format))
SDL2_SYMBOL(SDL_CreateWindow, SDL_Window *, (const char *title, int x, int y, int w, int h, Uint32 flags))
SDL2_SYMBOL(SDL_DestroyWindow, void, (SDL_Window *window))

SDL2_SYMBOL(SDL_CreateRGBSurface, SDL_Surface *, (Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask))
SDL2_SYMBOL(SDL_FreeSurface, void, (SDL_Surface *surface))
SDL2_SYMBOL(SDL_LockSurface, int, (SDL_Surface *surface))
SDL2_SYMBOL(SDL_UnlockSurface, void, (SDL_Surface *surface))

SDL2_SYMBOL(SDL_MapRGBA, Uint32, (const SDL_PixelFormat *format, Uint8 r, Uint8 g, Uint8 b, Uint8 a))

SDL2_SYMBOL(SDL_ShowCursor, int, (int toggle))
SDL2_SYMBOL(SDL_WarpMouseInWindow, void, (SDL_Window *window, int x, int y))

SDL2_SYMBOL(SDL_GetWindowGrab, SDL_bool, (SDL_Window *window))
SDL2_SYMBOL(SDL_SetWindowGrab, void, (SDL_Window *window, SDL_bool grabbed))
SDL2_SYMBOL(SDL_MinimizeWindow, void, (SDL_Window *window))
SDL2_SYMBOL(SDL_SetRelativeMouseMode, int, (SDL_bool enabled))

SDL2_SYMBOL(SDL_GL_LoadLibrary, int, (const char *path))
SDL2_SYMBOL(SDL_GL_GetProcAddress, void *, (const char *proc))
SDL2_SYMBOL(SDL_GL_SwapWindow, void, (SDL_Window *window))
SDL2_SYMBOL(SDL_GL_SetAttribute, int, (SDL_GLattr attr, int value))
SDL2_SYMBOL(SDL_GL_CreateContext, SDL_GLContext, (SDL_Window *window))
SDL2_SYMBOL(SDL_GL_DeleteContext, void, (SDL_GLContext context))

/* Audio subsystem */
SDL2_SYMBOL(SDL_OpenAudio, int, (SDL_AudioSpec *desired, SDL_AudioSpec *obtained))
SDL2_SYMBOL(SDL_PauseAudio, void, (int pause_on))

/* Events subsystem */
SDL2_SYMBOL(SDL_JoystickEventState, int, (int state))
SDL2_SYMBOL(SDL_GetModState, SDL_Keymod, (void))
SDL2_SYMBOL(SDL_PollEvent, int, (SDL_Event *event))

/* Joystick subsystem */
SDL2_SYMBOL(SDL_NumJoysticks, int, (void))
SDL2_SYMBOL(SDL_JoystickOpen, SDL_Joystick *, (int device_index))
SDL2_SYMBOL(SDL_JoystickName, const char *, (SDL_Joystick *joystick))
SDL2_SYMBOL(SDL_JoystickNameForIndex, const char *, (int index))
SDL2_SYMBOL(SDL_JoystickNumButtons, int, (SDL_Joystick *joystick))
SDL2_SYMBOL(SDL_JoystickNumAxes, int, (SDL_Joystick *joystick))
SDL2_SYMBOL(SDL_JoystickGetAxis, Sint16, (SDL_Joystick *joystick, int axis))
SDL2_SYMBOL(SDL_JoystickClose, void, (SDL_Joystick *joystick))
