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
#include "soundtools.h"
#include "datafile.h"

void (CALLBACKFUN *_endCallbackFunc)() = NULL;

#ifdef HAVE_LIBFMOD

#ifdef FMOD_HEADER
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#endif

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

void CMusic::update()
{;}

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

//Vorbisfile library
#ifdef HAVE_VORBISFILE
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"
#endif

CMusic::CMusic(CDataManager *manager) : CSndSample(manager)
{
	m_isLoaded = false;
	m_isStreaming = false;
	m_Source = 0;
}

CMusic::~CMusic()
{
#ifdef HAVE_LIBOPENAL
	if(m_isLoaded && m_isStreaming)
	{
#ifdef HAVE_VORBISFILE
		ov_clear(&m_VorbisFile);
#endif

		//Remove all streamed OpenALbuffers
		for(unsigned int i=0; i < m_StreamBuffers.size(); i++)
			alDeleteBuffers(1, &(m_StreamBuffers[i]));

		m_isLoaded = false;
	}
#endif
}

bool CMusic::load(const CString &filename, const CParamList &list)
{
#ifdef HAVE_VORBISFILE
	CString extension = filename.mid(filename.length() - 4);
	extension.toLower();
	if(extension == ".ogg")
	{
		//We have streaming support for Ogg vorbis
		m_isLoaded = true;
		m_isStreaming = true;
		m_streamIsFinished = false;

		CDataObject::load(filename, list);
		CDataFile f(m_Filename);
		CString realfile = f.useExtern();

		FILE *fp = fopen(realfile.c_str(), "r");
		if(ov_open(fp, &m_VorbisFile, NULL, 0) < 0)
		{
			printf("Input does not appear to be an Ogg bitstream.\n");
			m_isLoaded = false;
			fclose(fp);
			return m_isLoaded;
		}

		printf("    Loaded music file with streaming functionality\n");
		return m_isLoaded;
	}
#endif

	m_isStreaming = false;
	return CSndSample::load(filename, list);
}

int CMusic::attachToChannel(int c)
{
	if(m_isStreaming)
	{
		m_Source = c;

		//places first data in queue:
		for(unsigned int i=0; i<10; i++)
			update();

		return 0;
	}

	return CSndSample::attachToChannel(c);
}

void CMusic::update()
{
	if(m_isStreaming)
	{
		if(_endCallbackFunc != NULL)
		{
			checkForALError("before retrieving the music state");
			ALenum sourceState;
			alGetSourcei(m_Source, AL_SOURCE_STATE, &sourceState);
			checkForALError("when retrieving the music state");
			if(sourceState == AL_STOPPED)
			{
				printf("Music has stopped; starting next song..\n");
				_endCallbackFunc();
			}
		}

#ifdef HAVE_VORBISFILE
		if(!m_streamIsFinished)
		{
			const unsigned int bufferSize = 4096*16;
			char pcmData[bufferSize];
	
			unsigned int ret = loadVorbisChunk(
				&m_VorbisFile, pcmData, bufferSize);
	
			if (ret == 0)
			{
				m_streamIsFinished = true;
				printf("Finished loading Ogg music file (used %d buffers)\n",
					m_StreamBuffers.size());
			}
			else
			{
				//TODO: check for sample rate changes
				alGenBuffers(1, &m_Buffer);
				alBufferData(
					m_Buffer,
					AL_FORMAT_STEREO16,
					pcmData, ret,
					44100);
				alSourceQueueBuffers(m_Source, 1, &m_Buffer);
				checkForALError("when streaming music data");
				m_StreamBuffers.push_back(m_Buffer);
			}
		}
#endif
	}
}

void CMusic::setEndCallback(void (CALLBACKFUN *endfunc)())
{
	_endCallbackFunc = endfunc;
}

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

void CMusic::update()
{;}

void CMusic::setEndCallback(void (CALLBACKFUN *endfunc)())
{;}

#endif //libfmod and libopenal
