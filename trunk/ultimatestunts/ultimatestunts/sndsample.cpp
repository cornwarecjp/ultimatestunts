/***************************************************************************
                          sndsample.cpp  -  A 3D sound sample class
                             -------------------
    begin                : di feb 25 2003
    copyright            : (C) 2003 by CJP
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

#include "sndsample.h"

#include <stdio.h>
#ifdef HAVE_LIBFMOD

#ifdef FMOD_HEADER
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#endif

CSndSample::CSndSample()
{
	  m_Sample = NULL;
}

CSndSample::~CSndSample()
{
  FSOUND_Sample_Free(m_Sample);
  m_Sample = NULL;
}

int CSndSample::loadFromFile(CString filename)
{
	m_Sample = FSOUND_Sample_Load(FSOUND_FREE, filename.c_str(), FSOUND_HW3D, 0, 0);

	if (!m_Sample)
	{
		printf("   FMOD error: %s\n", FMOD_ErrorString(FSOUND_GetError()));
		return 1;
	}

	// increasing mindistnace makes it louder in 3d space
	FSOUND_Sample_SetMinMaxDistance((FSOUND_SAMPLE  *)m_Sample, 4.0f, 1000.0f);
	FSOUND_Sample_SetMode((FSOUND_SAMPLE  *)m_Sample, FSOUND_LOOP_NORMAL);

	return 0;
}

int CSndSample::attachToChannel(int c)
{
	return FSOUND_PlaySoundEx(c, m_Sample, NULL, true);
}

#else //libfmod

CSndSample::CSndSample()
{;}

CSndSample::~CSndSample()
{;}

int CSndSample::loadFromFile(CString filename)
{
  return 1;
}

int CSndSample::attachToChannel(int c)
{
  return -1; //no sound implementation
}

#endif //libfmod
