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

#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>

#include "sndsample.h"
#include "datafile.h"

#ifdef HAVE_LIBFMOD

#ifdef FMOD_HEADER
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#endif

CSndSample::CSndSample(CDataManager *manager) : CDataObject(manager, CDataObject::eSample)
{
	  m_Sample = NULL;
}

CSndSample::~CSndSample()
{
	FSOUND_Sample_Free(m_Sample);
}

bool CSndSample::load(const CString &filename, const CParamList &list)
{
	CDataObject::load(filename, list);
	CDataFile f(m_Filename);
	
	m_Sample = FSOUND_Sample_Load(FSOUND_FREE, f.useExtern().c_str(), FSOUND_HW3D, 0, 0);

	if (!m_Sample)
	{
		printf("   FMOD error: %s\n", FMOD_ErrorString(FSOUND_GetError()));
		return false;
	}

	// increasing mindistnace makes it louder in 3d space
	FSOUND_Sample_SetMinMaxDistance((FSOUND_SAMPLE  *)m_Sample, 4.0f, 1000.0f);
	FSOUND_Sample_SetMode((FSOUND_SAMPLE  *)m_Sample, FSOUND_LOOP_NORMAL);

	return true;
}

int CSndSample::attachToChannel(int c)
{
	return FSOUND_PlaySoundEx(c, m_Sample, NULL, true);
}

#elif defined HAVE_LIBOPENAL

#ifdef OPENAL_HEADER
#include <AL/al.h>
#include <AL/alut.h>
#endif

//Loki Ogg Vorbis function extension
#define VORBISFUNC "alutLoadVorbis_LOKI"
#define MP3FUNC "alutLoadMP3_LOKI"

CSndSample::CSndSample(CDataManager *manager) : CDataObject(manager, CDataObject::eSample)
{
	m_Buffer = 0;
	m_isLoaded = false;
}

CSndSample::~CSndSample()
{
	if(m_isLoaded)
	{
		alDeleteBuffers(1, &m_Buffer);
	}
}

bool CSndSample::load(const CString &filename, const CParamList &list)
{
	CDataObject::load(filename, list);
	CDataFile f(m_Filename);
	CString realfile = f.useExtern();

	//data
	void *wave;
	ALsizei format, size, bits, freq;

	CString extension = realfile.mid(realfile.length() - 4);
	extension.toLower();

	if(extension == ".ogg") //.ogg: load as ogg vorbis file
	{
		//the loading proc
		ALboolean (*alutLoadVorbis)(ALuint, ALvoid *, ALint) =
			(ALboolean (*)(ALuint, ALvoid *, ALint))alGetProcAddress((ALubyte *) VORBISFUNC);

		if(alutLoadVorbis == NULL)
		{
			printf("Error: Could not get ogg loading proc.\n"
				"Maybe you don't use the Loki openAL implementation?\n");
			return false;
		}

		//The data
		struct stat sbuf;
		stat(realfile.c_str(), &sbuf);
		size = sbuf.st_size;
		wave = malloc(size);
		FILE *fp = fopen(realfile.c_str(), "r");
		fread(wave, size, 1, fp);
		fclose(fp);
		
		alGenBuffers(1, &m_Buffer);
		if(alutLoadVorbis(m_Buffer, wave, size) != AL_TRUE)
			printf("alutLoadVorbis failed\n");
	}
	else if(extension == ".mp3") //.mp3: load as MP3 file
	{
		//the loading proc
		ALboolean (*alutLoadMP3)(ALuint, ALvoid *, ALint) =
			(ALboolean (*)(ALuint, ALvoid *, ALint))alGetProcAddress((ALubyte *) MP3FUNC);

		if(alutLoadMP3 == NULL)
		{
			printf("Error: Could not get MP3 loading proc.\n"
				"Maybe you don't use the Loki openAL implementation?\n");
			return false;
		}

		//The data
		struct stat sbuf;
		stat(realfile.c_str(), &sbuf);
		size = sbuf.st_size;
		wave = malloc(size);
		FILE *fp = fopen(realfile.c_str(), "r");
		fread(wave, size, 1, fp);
		fclose(fp);

		alGenBuffers(1, &m_Buffer);
		if(alutLoadMP3(m_Buffer, wave, size) != AL_TRUE)
			printf("alutLoadMP3 failed\n");
	}
	else //default: load as wave file
	{
		alGenBuffers(1, &m_Buffer);
		alutLoadWAV(realfile.c_str(), &wave, &format, &size, &bits, &freq);
		alBufferData(m_Buffer, format, wave, size, freq);
	}

	free(wave);
	m_isLoaded = true;
	return true;
}

int CSndSample::attachToChannel(int c)
{
	alSourcei((unsigned int)c, AL_BUFFER, m_Buffer);
	return 0;
}

#else //libfmod and libopenAL


CSndSample::CSndSample(CDataManager *manager) : CDataObject(manager, CDataObject::eSample)
{;}

CSndSample::~CSndSample()
{;}

bool CSndSample::load(const CString &filename, const CParamList &list)
{
	CDataObject::load(filename, list);
	CDataFile f(m_Filename);

	return true;
}

int CSndSample::attachToChannel(int c)
{
	return -1; //no sound implementation
}

#endif //libfmod
