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
#include "soundtools.h"
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

#ifdef HAVE_LIBOPENAL

#ifdef HAVE_VORBISFILE
unsigned int CSndSample::loadVorbisChunk(OggVorbis_File *vf, char *buffer, unsigned int bufSize)
{
	int current_section;
	char *readPtr = buffer;
	int remainingBytes = (int)bufSize;
	while(remainingBytes > 0)
	{
		long ret = ov_read(
			vf, //file info object
			readPtr, //output buffer
			remainingBytes, //size of output buffer
			0, //0 = little endian
			2, //2 bytes per sample
			1, //1 = signed data
			&current_section //logical bitstream
			);
		if (ret == 0)
		{
			break; //end of file
		}
		else if (ret < 0)
		{
			printf("Received error %d while reading Ogg stream\n", (int)ret);
		}
		else
		{
			//TODO: check for sample rate changes
			readPtr += ret;
			remainingBytes -= ret;
		}
	}

	return (unsigned int)(readPtr - buffer);
}
#endif

void CSndSample::loadOgg(const CString &filename)
{
	m_isLoaded = true;

	//data (and default values)
	void *wave = NULL;
	ALsizei size = 0;

	//the loading extention from ALUT
	ALboolean (*alutLoadVorbis)(ALuint, ALvoid *, ALint) =
		(ALboolean (*)(ALuint, ALvoid *, ALint))
			alGetProcAddress((const ALchar *)VORBISFUNC);

	if(alutLoadVorbis != NULL)
	{
		//The data
		struct stat sbuf;
		stat(filename.c_str(), &sbuf);
		size = sbuf.st_size;
		wave = malloc(size);
		FILE *fp = fopen(filename.c_str(), "r");
		if(fread(wave, size, 1, fp) != (size_t)size)
		{
			printf("Failed to read from file\n");
		}
		fclose(fp);

		alGenBuffers(1, &m_Buffer);
		if(alutLoadVorbis(m_Buffer, wave, size) != AL_TRUE)
		{
			printf("alutLoadVorbis failed\n");
			alDeleteBuffers(1, &m_Buffer);
			m_isLoaded = false;
		}

		free(wave);
	}
	else
#ifdef HAVE_VORBISFILE
	{
		//Load an Ogg vorbis file into OpenAL,
		//using libvorbisfile

		m_isLoaded = true;

		FILE *fp = fopen(filename.c_str(), "r");
		OggVorbis_File vf;
		if(ov_open(fp, &vf, NULL, 0) < 0)
		{
			printf("Input does not appear to be an Ogg bitstream.\n");
			m_isLoaded = false;
			fclose(fp);
			return;
		}

		ogg_int64_t size = ov_pcm_total(&vf, -1);

		int numBytes = 4*size; //16bit stereo
		char *pcmData = new char[numBytes];

		loadVorbisChunk(&vf, pcmData, numBytes);

		alGenBuffers(1, &m_Buffer);
		alBufferData(
			m_Buffer,
			AL_FORMAT_STEREO16,
			pcmData, numBytes,
			44100); //TODO: get sample rate from Ogg file

		delete pcmData;

		ov_clear(&vf);
		//ov_clear has already called fclose(fp)
	}
#else
	{
		printf("Warning: trying to load a .ogg file,"
			"but ALUT ogg extention is not present.\n");
		loadOther(filename);
	}
#endif
}

void CSndSample::loadMP3(const CString &filename)
{
	m_isLoaded = true;

	//data (and default values)
	void *wave = NULL;
	ALsizei size = 0;

	//the loading extention from ALUT
	ALboolean (*alutLoadMP3)(ALuint, ALvoid *, ALint) =
		(ALboolean (*)(ALuint, ALvoid *, ALint))
			alGetProcAddress((const ALchar *)MP3FUNC);

	if(alutLoadMP3 != NULL)
	{
		//The data
		struct stat sbuf;
		stat(filename.c_str(), &sbuf);
		size = sbuf.st_size;
		wave = malloc(size);
		FILE *fp = fopen(filename.c_str(), "r");
		if(fread(wave, size, 1, fp) != (size_t)size)
		{
			printf("Failed to read from file\n");
		}
		fclose(fp);

		alGenBuffers(1, &m_Buffer);
		if(alutLoadMP3(m_Buffer, wave, size) != AL_TRUE)
		{
			printf("alutLoadMP3 failed\n");
			alDeleteBuffers(1, &m_Buffer);
			m_isLoaded = false;
		}

		free(wave);
	}
	else
	{
		printf("Warning: trying to load a .mp3 file,"
			"but ALUT mp3 extention is not present.\n");
		loadOther(filename);
	}
}

void CSndSample::loadOther(const CString &filename)
{
	m_isLoaded = true;

	m_Buffer = alutCreateBufferFromFile(filename.c_str());

	if(m_Buffer == AL_NONE)
	{
		printf("    Failed to load %s\n", filename.c_str());
		printf("    ALUT error was: %s\n", alutGetErrorString(alutGetError()));

		m_Buffer = 0;
		m_isLoaded = false;
	}
}

#endif

bool CSndSample::load(const CString &filename, const CParamList &list)
{
	CDataObject::load(filename, list);
	CDataFile f(m_Filename);

#ifdef HAVE_LIBFMOD
	m_isLoaded = true;

	m_Sample = FSOUND_Sample_Load(FSOUND_FREE, f.useExtern().c_str(), FSOUND_HW3D, 0, 0);

	if (!m_Sample)
	{
		printf("   FMOD error: %s\n", FMOD_ErrorString(FSOUND_GetError()));
		m_isLoaded = false;
		return m_isLoaded;
	}

	// increasing mindistnace makes it louder in 3d space
	FSOUND_Sample_SetMinMaxDistance((FSOUND_SAMPLE  *)m_Sample, 4.0f, 1000.0f);
	FSOUND_Sample_SetMode((FSOUND_SAMPLE  *)m_Sample, FSOUND_LOOP_NORMAL);
#endif

#ifdef HAVE_LIBOPENAL
	CString realfile = f.useExtern();

	CString extension = realfile.mid(realfile.length() - 4);
	extension.toLower();

	//Now remove AL errors from previous calls
	checkForALError("before loading");

	if(extension == ".ogg") //.ogg: load as ogg vorbis file
	{
		loadOgg(realfile);
	}
	else if(extension == ".mp3") //.mp3: load as MP3 file
	{
		loadMP3(realfile);
	}
	else
	{
		loadOther(realfile);
	}

	//Check for errors
	checkForALError("after loading");

#endif

	return m_isLoaded;
}

int CSndSample::attachToChannel(int c)
{
#ifdef HAVE_LIBFMOD
	return FSOUND_PlaySoundEx(c, m_Sample, NULL, 1);
#endif

#ifdef HAVE_LIBOPENAL
	if(m_isLoaded)
		alSourcei((unsigned int)c, AL_BUFFER, m_Buffer);
	return 0;
#endif

	return -1;
}
