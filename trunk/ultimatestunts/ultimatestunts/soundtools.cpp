/***************************************************************************
                          soundtools.cpp  -  Some tool functions that can be used in the sound code
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

#include <cstdio>

#include "soundtools.h"

#ifdef HAVE_LIBOPENAL

void checkForALError(const CString &context)
{
	ALenum err = alGetError();
	if(err != AL_NO_ERROR)
	{
		printf("An OpenAL error occurred %s:\n", context.c_str());
		printf("  %s\n", alGetString(err));
	}
}

#endif

