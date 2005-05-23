/***************************************************************************
                          sound.cpp  -  The basic sound system
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <cmath>
#include <cstdio> //debugging

#include "sound.h"

#include "vector.h"
#include "usmacros.h"
#include "datafile.h"
#include "car.h"

bool _song_has_ended;

void musicEndCallback()
{
	//_theSoundObject->playNextSong();
	//printf("musicEndCallback done\n");
	_song_has_ended = true;
}

#ifdef FMOD_HEADER
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#include <fmod/wincompat.h> //debugging
#endif

#ifdef OPENAL_HEADER
#include <AL/al.h>
#include <AL/alut.h>
#endif


CSound::CSound(const CLConfig &conf)
{
	//The world object:
	m_World = theWorld;

#ifdef HAVE_LIBOPENAL
	int argc = 1;
	char *argv[1];
	argv[0] = "ultimatestunts";
	alutInit(&argc, argv);

	alListenerf(AL_GAIN, 1.0);
	alDopplerFactor(1.0);
	alDopplerVelocity(340.0);
#endif

#ifdef HAVE_LIBFMOD
	//Initialising the sound library:
	CString drivername = "nosound"; //Default for unknown system types

#ifdef __CYGWIN__
	drivername = conf.getValue("sound", "windowsdriver");
#elif defined(__linux__)
	drivername = conf.getValue("sound", "linuxdriver");
#endif

	if (FSOUND_GetVersion() < FMOD_VERSION)
	{
		printf("   Error : You are using the wrong DLL version!  You should be using FMOD %.02f\n", FMOD_VERSION);
		return;
	}

  // SELECT OUTPUT METHOD
	if(drivername == "directsound")
		FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
	else if(drivername == "winmm")
		FSOUND_SetOutput(FSOUND_OUTPUT_WINMM);
	else if(drivername == "a3d")
		FSOUND_SetOutput(FSOUND_OUTPUT_A3D);
	else if(drivername == "oss")
		FSOUND_SetOutput(FSOUND_OUTPUT_OSS);
	else if(drivername == "alsa")
		FSOUND_SetOutput(FSOUND_OUTPUT_ALSA);
	else if(drivername == "esd")
		FSOUND_SetOutput(FSOUND_OUTPUT_ESD);
	else if(drivername == "nosound")
		FSOUND_SetOutput(FSOUND_OUTPUT_NOSOUND);
	else
	{
		printf("   Unknown driver name %s: using nosound driver\n", drivername.c_str());
		FSOUND_SetOutput(FSOUND_OUTPUT_NOSOUND);
	}

	// SELECT DRIVER
	if(FSOUND_GetNumDrivers() > 1)
	{
		CString subdriver = conf.getValue("sound", "subdriver");
		printf("   Selected driver: %s\n", subdriver.c_str());
		printf("   Available drivers:\n");
		for (int i=0; i < FSOUND_GetNumDrivers(); i++)
		{
			printf("   %d: %s\n", i+1, FSOUND_GetDriverName(i));

			if(subdriver == (char *)FSOUND_GetDriverName(i))
				FSOUND_SetDriver(i);
		}
	}

	printf("   Loaded driver: %s\n", FSOUND_GetDriverName(FSOUND_GetDriver()) );

  // INITIALIZE
  if (!FSOUND_Init(44100, 32, 0))
  {
    printf("  Init: %s\n", FMOD_ErrorString(FSOUND_GetError()));
    return;
  }
#endif //libfmod


	//The sound world:
	m_SoundWorld = new CSoundWorld(conf);

	m_MusicVolume = conf.getValue("sound", "musicvolume").toInt();
	m_SoundVolume = conf.getValue("sound", "soundvolume").toInt();

	//Defining the playlist
	vector<CString> wavfiles = getDirContents("music", ".wav");
	vector<CString> mp3files = getDirContents("music", ".mp3");
	vector<CString> oggfiles = getDirContents("music", ".ogg");
	m_Playlist = wavfiles;
	for(unsigned int i=0; i < mp3files.size(); i++)
		m_Playlist.push_back(mp3files[i]);
	for(unsigned int i=0; i < oggfiles.size(); i++)
		m_Playlist.push_back(oggfiles[i]);
	for(unsigned int i=0; i < m_Playlist.size(); i++)
		m_Playlist[i] = CString("music/") + m_Playlist[i];

	//Loading data:
	m_Music = NULL;
	m_MusicObject = NULL;
	playNextSong();
}

CSound::~CSound()
{
	unload();

	if(m_MusicObject != NULL && m_Music != NULL)
	{
		m_Music->setEndCallback(NULL);
		delete m_MusicObject;
		delete m_Music;
	}

#ifdef HAVE_LIBFMOD
	FSOUND_Close();
#endif

#ifdef HAVE_LIBOPENAL
	alutExit();
#endif
}

bool CSound::load()
{
	return m_SoundWorld->loadObjects();
}

void CSound::unload()
{
	m_SoundWorld->unloadObjects();
}

void CSound::playNextSong()
{
	if(m_MusicObject != NULL && m_Music != NULL)
	{
		m_Music->setEndCallback(NULL);
		delete m_MusicObject;
		delete m_Music;
	}

	if(m_Playlist.size() == 0) return; //just don't play music

	m_Music = new CMusic(NULL);
	m_MusicObject = new CSoundObj(-1);

	m_PlaylistItem++;
	if(m_PlaylistItem >= m_Playlist.size()) m_PlaylistItem = 0;

	//printf("   Loading music file %s\n\n", m_Playlist[m_PlaylistItem].c_str());
	m_Music->load(m_Playlist[m_PlaylistItem], CParamList());

	//printf("setSample\n");
	m_MusicObject->setSample(m_Music);
	//printf("setEndCallback\n");
#ifndef __CYGWIN__
	m_Music->setEndCallback(musicEndCallback);
#endif
	//printf("setVolume\n");
	m_MusicObject->setVolume(m_MusicVolume);
	//printf("done\n");
	_song_has_ended = false;
}

void CSound::update()
{
	//Music:
	if(_song_has_ended)
		playNextSong();

	//Microphone:
	CVector p = m_Camera->getPosition();
	CVector v = m_Camera->getVelocity();
	const CMatrix &ori = m_Camera->getOrientation();

#ifdef HAVE_LIBOPENAL
	//Arguments: pos, vel, front x,y,z, top x,y,z
	float ori_arr[] =
		{-ori.Element(0,2), -ori.Element(1,2), -ori.Element(2,2),
		  ori.Element(0,1),  ori.Element(1,1),  ori.Element(2,1)};
	alListener3f(AL_POSITION, p.x/10, p.y/10, p.z/10);
	alListener3f(AL_VELOCITY, v.x/10, v.y/10, v.z/10);
	alListenerfv(AL_ORIENTATION, ori_arr);
#endif

#ifdef HAVE_LIBFMOD
	float pos[] = {p.x/10, p.y/10, -p.z/10};
	float vel[] = {v.x/10, v.y/10, -v.z/10};
	//Arguments: pos, vel, front x,y,z, top x,y,z
	FSOUND_3D_Listener_SetAttributes(
		&pos[0], &vel[0],
		-ori.Element(0,2), -ori.Element(1,2),  ori.Element(2,2),
		 ori.Element(0,1),  ori.Element(1,1), -ori.Element(2,1)
	);
#endif


	//Objects:
	for(unsigned int i=0; i<m_SoundWorld->m_Channels.size(); i++)
	{
		CSoundObj *chn = m_SoundWorld->m_Channels[i];
		const CMovingObject *o = m_World->getMovingObject(chn->getMovingObjectID());
		if(o->getType() == CMessageBuffer::car)
		{
			CCar *theCar = (CCar *)o;
			CVector v = theCar->m_Velocity;
			chn->setPosVel(theCar->m_Position, v);

			switch(chn->m_SoundType)
			{
			case CSoundObj::eSkid:
			{
				unsigned int vol = 0;
				
				for(unsigned int w=0; w < 4; w++)
					vol += int(63 * theCar->m_Wheel[i].m_SkidVolume);
				
				chn->setVolume((vol * m_SoundVolume) >> 8);
				break;
			}
			case CSoundObj::eEngine:
			{
				float engineRPS = theCar->m_Engine.m_MainAxisW * theCar->m_Engine.getGearRatio();
				int vol = 100 + (int)(100 * theCar->m_Engine.m_Gas);
				if(vol > 255) vol = 255;
				chn->setFrequency(0.0025 * engineRPS); //correct for sound sample frequency & 2*pi
				chn->setVolume((vol * m_SoundVolume) >> 8);
				break;
			}
			case CSoundObj::eCrash:
			{
				bool doCrash = false;
				float intensity = 1.0;
				for(unsigned int j=0; j < o->m_AllCollisions.size(); j++)
				{
					doCrash = true;
					//const CCollisionData &coll = o->m_Collisions[j];
				}
				if(doCrash)
				{
					chn->setVolume(int(255*intensity));
					chn->playOnce();
				}
				break;
			}
			}
		}
	}

#ifdef HAVE_LIBFMOD
	//Update:
	FSOUND_Update();
#endif
}
