/***************************************************************************
                          usmacros.h  -  ultimate stunts macros
                             -------------------
    copyright            : (C) 2002 by bones
    email                : boesemar@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _USENDIAN_H
#define _USENDIAN_H

#include <SDL/SDL_endian.h>
#include <SDL/SDL.h>

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define ENDIANFIX16(X)    (X)
#define ENDIANFIX32(X)    (X)
#else
#define ENDIANFIX16(X)    SDL_Swap16(X)
#define ENDIANFIX32(X)    SDL_Swap32(X)
#endif

#define LOBYTE(w)           ((Uint8)(w))
#define HIBYTE(w)           ((Uint8)(((Uint16)(w) >> 8) & 0xFF))



#endif
