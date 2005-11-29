/***************************************************************************
                          music.cpp  -  A Music class
                             -------------------
    begin                : wo aug 13 2003
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

#include <cstdio>

#include "music.h"
#include "datafile.h"

#ifdef HAVE_LIBFMOD

#ifdef FMOD_HEADER
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#endif

void (CALLBACKFUN *_endCallbackFunc)();
bool _true = true;
bool _false = false;

F_CALLBACKAPI signed char endCallback(FSOUND_STREAM *stream, void *buff, int len, void *param)
{
	//printf("endCallback:\n");

	if( *((bool *)param) && _endCallbackFunc != NULL)
		_endCallbackFunc();

	//printf("...endCallback done\n");
	return false;
}

CMusic::CMusic(CDataManager *manager) : CSndSample(manager)
{
	  m_Stream = NULL;
}

CMusic::~CMusic()
{
	if(m_Stream != NULL) FSOUND_Stream_Close(m_Stream);
}

bool CMusic::load(const CString &filename, const CParamList &list)
{
	CDataObject::load(filename, list);
	CDataFile f(m_Filename);
	
	m_Stream = FSOUND_Stream_Open(f.useExtern().c_str(), FSOUND_NORMAL, 0, 0);

	if (!m_Stream)
	{
		printf("   FMOD error: %s\n", FMOD_ErrorString(FSOUND_GetError()));
		return false;
	}

	return true;
}

int CMusic::attachToChannel(int c)
{
  return FSOUND_Stream_PlayEx(c, m_Stream, NULL, true);
}

void CMusic::setEndCallback(void (CALLBACKFUN *endfunc)())
{
	if(endfunc == NULL) //disable callback
	{
		FSOUND_Stream_SetEndCallback(m_Stream, endCallback, &_false);
		_endCallbackFunc = NULL;
	}
	else
	{
		_endCallbackFunc = endfunc;
		FSOUND_Stream_SetEndCallback(m_Stream, endCallback, &_true);
	}
}

#elif defined HAVE_LIBOPENAL

#ifdef OPENAL_HEADER
#include <AL/al.h>
#include <AL/alut.h>
#endif

CMusic::CMusic(CDataManager *manager) : CSndSample(manager)
{
}

CMusic::~CMusic()
{
}

bool CMusic::load(const CString &filename, const CParamList &list)
{
	//TODO: streaming
	return CSndSample::load(filename, list);
}

int CMusic::attachToChannel(int c)
{
	return CSndSample::attachToChannel(c);
}

void CMusic::setEndCallback(void (CALLBACKFUN *endfunc)())
{;}

#else //libfmod and libopenal

CMusic::CMusic(CDataManager *manager) : CSndSample(manager)
{;}

CMusic::~CMusic()
{;}

bool CMusic::load(const CString &filename, const CParamList &list)
{
  return true;
}

int CMusic::attachToChannel(int c)
{
  return -1; //no sound implementation
}

void CMusic::setEndCallback(void (CALLBACKFUN *endfunc)())
{;}

#endif //libfmod
