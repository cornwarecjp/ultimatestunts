/***************************************************************************
                          soundtools.h  -  Some tool functions that can be used in the sound code
                             -------------------
    begin                : di dec 21 2010
    copyright            : (C) 2010 by CJP
    email                : cornware-cjp@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef SOUNDTOOLS_H
#define SOUNDTOOLS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_LIBOPENAL

#ifdef OPENAL_HEADER
#include <AL/al.h>
#include <AL/alut.h>
#endif

#endif

#include "cstring.h"

#ifdef HAVE_LIBOPENAL

void checkForALError(const CString &context);

#endif

#endif


