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

#endif


#ifdef HAVE_LIBOPENAL

#ifdef OPENAL_HEADER
#include <AL/al.h>
#include <AL/alut.h>
#endif

//Loki Ogg Vorbis function extension
#define VORBISFUNC "alutLoadVorbis_LOKI"
#define MP3FUNC "alutLoadMP3_LOKI"

#endif


CSndSample::CSndSample(CDataManager *manager) : CDataObject(manager, CDataObject::eSample)
{
#ifdef HAVE_LIBFMOD
	  m_Sample = NULL;
#endif

#ifdef HAVE_LIBOPENAL
	m_Buffer = 0;
#endif
}

CSndSample::~CSndSample()
{
	if(m_isLoaded)
	{
#ifdef HAVE_LIBFMOD
		FSOUND_Sample_Free(m_Sample);
#endif

#ifdef HAVE_LIBOPENAL
		alDeleteBuffers(1, &m_Buffer);
#endif
	}
}

bool CSndSample::load(const CString &filename, const CParamList &list)
{
	CDataObject::load(filename, list);
	CDataFile f(m_Filename);

#ifdef HAVE_LIBFMOD
	m_Sample = FSOUND_Sample_Load(FSOUND_FREE, f.useExtern().c_str(), FSOUND_HW3D, 0, 0);

	if (!m_Sample)
	{
		printf("   FMOD error: %s\n", FMOD_ErrorString(FSOUND_GetError()));
		return false;
	}

	// increasing mindistnace makes it louder in 3d space
	FSOUND_Sample_SetMinMaxDistance((FSOUND_SAMPLE  *)m_Sample, 4.0f, 1000.0f);
	FSOUND_Sample_SetMode((FSOUND_SAMPLE  *)m_Sample, FSOUND_LOOP_NORMAL);
#endif

#ifdef HAVE_LIBOPENAL
	CString realfile = f.useExtern();

	//data (and default values)
	void *wave = NULL;
	ALsizei size = 0;

	CString extension = realfile.mid(realfile.length() - 4);
	extension.toLower();

	//the loading procs
	ALboolean (*alutLoadVorbis)(ALuint, ALvoid *, ALint) =
		(ALboolean (*)(ALuint, ALvoid *, ALint))
			alGetProcAddress(VORBISFUNC);

	ALboolean (*alutLoadMP3)(ALuint, ALvoid *, ALint) =
		(ALboolean (*)(ALuint, ALvoid *, ALint))
			alGetProcAddress(MP3FUNC);

	//Now remove AL errors from previous calls
	{
		ALenum err = alGetError();
		if(err != AL_NO_ERROR)
		{
			printf("An OpenAL error occurred before loading %s:\n", filename.c_str());
			printf("  %s\n", alGetString(err));
		}
	}


	if(alutLoadVorbis != NULL && extension == ".ogg") //.ogg: load as ogg vorbis file
	{
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
	else if(alutLoadMP3 != NULL && extension == ".mp3") //.mp3: load as MP3 file
	{
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
	else //default ALUT loader
	{
		m_Buffer = alutCreateBufferFromFile(realfile.c_str());

		if(m_Buffer == AL_NONE)
		{
			printf("    Failed to load %s\n", realfile.c_str());
			printf("    ALUT error was: %s\n", alutGetErrorString(alutGetError()));

			m_Buffer = 0;
			m_isLoaded = false;
			return false;
		}

		/*
		ALsizei
			format = AL_FORMAT_MONO16,
			bits = 0,
			freq = 44100;

		alGenBuffers(1, &m_Buffer);
		alutLoadWAV(realfile.c_str(), &wave, &format, &size, &bits, &freq);
		alBufferData(m_Buffer, format, wave, size, freq);
		*/
	}

	free(wave);
	m_isLoaded = true;

	//Check for errors
	{
		ALenum err = alGetError();
		if(err != AL_NO_ERROR)
		{
			printf("An OpenAL error occurred when loading %s:\n", filename.c_str());
			printf("  %s\n", alGetString(err));
		}
	}

#endif

	return true;
}

int CSndSample::attachToChannel(int c)
{
#ifdef HAVE_LIBFMOD
	return FSOUND_PlaySoundEx(c, m_Sample, NULL, 1);
#endif

#ifdef HAVE_LIBOPENAL
	alSourcei((unsigned int)c, AL_BUFFER, m_Buffer);
	return 0;
#endif

	return -1;
}
