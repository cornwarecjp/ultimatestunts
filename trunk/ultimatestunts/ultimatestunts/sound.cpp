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
#include <cstdlib>
#include <cmath>
#include <cstdio> //debugging

#include "sound.h"

#include "vector.h"
#include "usmacros.h"
#include "datafile.h"
#include "car.h"

CSound *_theSoundObject;
bool _song_has_ended;

void musicEndCallback()
{
	//_theSoundObject->playNextSong();
	//printf("musicEndCallback done\n");
	_song_has_ended = true;
}

#ifdef HAVE_LIBFMOD

#ifdef FMOD_HEADER
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#include <fmod/wincompat.h> //debugging
#endif

CSound::CSound(const CLConfig &conf, const CWorld *world)
{
	_theSoundObject = this;

	//The world object:
	m_World = world;

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

	//The sound world:
	m_SoundWorld = new CSoundWorld(world, conf);

	m_MusicVolume = conf.getValue("sound", "musicvolume").toInt();
	m_SoundVolume = conf.getValue("sound", "soundvolume").toInt();

	//Defining the playlist
	{
		CDataFile f(conf.getValue("sound", "playlist"));
		while(true)
		{
			CString line = f.readl();
			if(line == "\n") break;
			line = line.Trim();
			if(line != "")
				m_Playlist.push_back(line);
		}
	}
	m_PlaylistItem = m_Playlist.size()-1;

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

	FSOUND_Close();
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

	m_Music = new CMusic;
	m_MusicObject = new CSoundObj;

	m_PlaylistItem++;
	if(m_PlaylistItem >= m_Playlist.size()) m_PlaylistItem = 0;

	CDataFile f(m_Playlist[m_PlaylistItem]);
	printf("   Loading music file %s\n\n", f.getName().c_str());
	m_Music->loadFromFile(f.useExtern());

	//printf("setSample\n");
	m_MusicObject->setSample(m_Music);
	//printf("setEndCallback\n");
	m_Music->setEndCallback(musicEndCallback);
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
	float pos[] = {p.x/10, p.y/10, p.z/10};
	CVector v = m_Camera->getVelocity();
	float vel[] = {v.x/10, v.y/10, v.z/10};
	const CMatrix &ori = m_Camera->getOrientation();

	//Arguments: pos, vel, front x,y,z, top x,y,z
	FSOUND_3D_Listener_SetAttributes(
		&pos[0], &vel[0],
		-ori.Element(0,2), ori.Element(1,2), ori.Element(2,2),
		-ori.Element(0,1), ori.Element(1,1), ori.Element(2,1)
	);


	//Objects:
	unsigned int s = m_SoundWorld->m_Channels.size();
	for(unsigned int i=0; i<s; i++)
	{
		CMovingObject *o = m_World->m_MovObjs[m_SoundWorld->m_ObjIDs[i]];
		if(o->getType() == CMessageBuffer::car)
		{
			CCar *theCar = (CCar *)o;
			CSoundObj *chn = m_SoundWorld->m_Channels[i];
			CVector v = theCar->m_Bodies[0].getVelocity();
			chn->setPosVel(theCar->m_Bodies[0].getPosition(), v);
			float engineRPM = theCar->m_MainAxisVelocity / theCar->getGearRatio();
			int vol = 127 + (int)(128 * theCar->m_gas);
			//fprintf(stderr, "Setting vol,freq of %d to %d,%3.3f\n", i, vol, freq);
			chn->setFrequency(0.005 * engineRPM); //correct for sound sample frequency
			chn->setVolume((vol * m_SoundVolume) >> 8);
		}
	}

	//Update:
	FSOUND_Update();
}

#else

CSound::CSound(const CLConfig &conf, const CWorld *world)
{
	printf("   No sound: sound support not compiled in\n");
}

CSound::~CSound()
{;}

bool CSound::load()
{return true;}

void CSound::playNextSong()
{;}

void CSound::update()
{;}
#endif //libfmod
