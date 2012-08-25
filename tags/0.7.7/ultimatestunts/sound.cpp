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

#ifdef FMOD_HEADER
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#endif

#ifdef OPENAL_HEADER
#include <AL/al.h>
#include <AL/alut.h>
#endif

#include "sound.h"
#include "soundtools.h"

#include "vector.h"
#include "usmacros.h"
#include "datafile.h"
#include "car.h"
#include "world.h"

#include "lconfig.h"

bool _song_has_ended;

void musicEndCallback()
{
	//_theSoundObject->playNextSong();
	//printf("musicEndCallback done\n");
	_song_has_ended = true;
}

CSound::CSound()
{
#ifdef HAVE_LIBOPENAL
	int argc = 1;
	char *argv[1];
	argv[0] = (char *)"ultimatestunts";
	ALboolean ret = alutInit(&argc, argv);

	if(ret == AL_FALSE)
	{
		printf("    Error: ALUT OpenAL initialization failed\n");
	}

	alListenerf(AL_GAIN, 1.0);
	alSpeedOfSound(343.3);
	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
#endif

#ifdef HAVE_LIBFMOD
	CLConfig &conf = *theMainConfig;

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

	FSOUND_3D_SetRolloffFactor(1.0);
#endif //libfmod


	//The sound world:
	m_SoundWorld = new CSoundWorld();

	m_Music = NULL;
	m_MusicObject = NULL;

	reloadConfiguration();

	m_Camera = NULL;

	//Loading music data:
	playNextSong();
}

bool CSound::reloadConfiguration()
{
	CLConfig &conf = *theMainConfig;

	//TODO: driver settings

#ifdef HAVE_LIBOPENAL
	//OpenAL doppler is not yet bug-free:
	alDopplerFactor(conf.getValue("workaround", "openal_008_dopplerfactor").toFloat());
#endif

	m_MusicVolume = conf.getValue("sound", "musicvolume").toInt();
	m_SoundVolume = conf.getValue("sound", "soundvolume").toInt();

	if(m_MusicObject != NULL) m_MusicObject->setVolume(m_MusicVolume);

	//Re-checking music playlist
	//Defining the playlist
	vector<CString> wavfiles = getDataDirContents("music", ".wav");
	vector<CString> mp3files = getDataDirContents("music", ".mp3");
	vector<CString> oggfiles = getDataDirContents("music", ".ogg");
	m_Playlist = wavfiles;
	for(unsigned int i=0; i < mp3files.size(); i++)
		m_Playlist.push_back(mp3files[i]);
	for(unsigned int i=0; i < oggfiles.size(); i++)
		m_Playlist.push_back(oggfiles[i]);
	for(unsigned int i=0; i < m_Playlist.size(); i++)
		m_Playlist[i] = CString("music/") + m_Playlist[i];

	return true;
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
	m_Camera = NULL;
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
	m_MusicObject = new CSoundObj(-1, false); //false = not looping

	m_PlaylistItem++;
	if(m_PlaylistItem >= m_Playlist.size()) m_PlaylistItem = 0;

	//printf("   Loading music file %s\n\n", m_Playlist[m_PlaylistItem].c_str());
	m_Music->load(m_Playlist[m_PlaylistItem], CParamList());

	m_MusicObject->setSample(m_Music);
	m_Music->setEndCallback(musicEndCallback);
	m_MusicObject->setVolume(m_MusicVolume);
	m_MusicObject->playOnce();
	_song_has_ended = false;
}

void CSound::update()
{
	//Music:
	m_Music->update();
	if(_song_has_ended)
		playNextSong();

	//Abuse the camera pointer to see whether we
	//are in-game or in-GUI.
	//If in-GUI, only the music needs attention.
	if(m_Camera == NULL)
		return;

	//Microphone:
	CVector p = m_Camera->getPosition();
	CVector v = m_Camera->getVelocity();
	const CMatrix &ori = m_Camera->getOrientation();

#ifdef HAVE_LIBOPENAL
	float ori_arr[] =
		{-ori.Element(2,0), -ori.Element(2,1), -ori.Element(2,2),
		  ori.Element(1,0),  ori.Element(1,1),  ori.Element(1,2)};
	alListener3f(AL_POSITION, p.x, p.y, p.z);
	alListener3f(AL_VELOCITY, v.x, v.y, v.z);
	alListenerfv(AL_ORIENTATION, ori_arr);

	if(theWorld != NULL)
	{
		if(theWorld->m_Paused && theWorld->m_LastTime > 0.0)
			{alListenerf(AL_GAIN, 0.0);} //mute all when paused
		else
			{alListenerf(AL_GAIN, 1.0);}
	}

	//Debug:
	/*
	{
		CSoundObj *chn = m_SoundWorld->m_Channels[0];
		const CMovingObject *o = theWorld->getMovingObject(chn->getMovingObjectID());
		CVector SV = o->m_Velocity;
		CVector LV = m_Camera->getVelocity();
		CVector SL = m_Camera->getPosition() - o->m_Position;
		float SS = 343.3;
		float DF = 1.0;

		float vls = SL.dotProduct(LV) / SL.abs();
		float vss = SL.dotProduct(SV) / SL.abs();

		float fmul = (SS - DF*vls) / (SS - DF*vss);
		printf("fmul = %f\n", fmul);

		//printf("dv = %s\n", CString(vCam - vObj).c_str());
	}
	*/
#endif

#ifdef HAVE_LIBFMOD
	float pos[] = {p.x, p.y, -p.z};
	float vel[] = {v.x, v.y, -v.z};
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
		const CMovingObject *o = theWorld->getMovingObject(chn->getMovingObjectID());
		if(o->getType() == CMessageBuffer::car)
		{
			CCar *theCar = (CCar *)o;
			CVector v = theCar->m_Velocity;
			chn->setPosVel(theCar->m_Position, v);

			switch(chn->m_SoundType)
			{
			case CSoundObj::eSkid:
			{
				float vol = 0;
				
				for(unsigned int w=0; w < 4; w++)
				{
					vol += 0.2 * theCar->m_Wheel[w].m_SkidVolume;
				}

				chn->setVolume(int(vol * m_SoundVolume));
				break;
			}
			case CSoundObj::eEngine:
			{
				float engineRPS = theCar->m_Engine.m_MainAxisW * theCar->m_Engine.getGearRatio();
				float vol = 0.2 + 0.4 * theCar->m_Engine.m_Gas;
				if(vol > 1.0) vol = 1.0;
				chn->setFrequency(fabsf(engineRPS / theCar->m_EngineSoundBaseRPS));
				chn->setVolume(int(vol * m_SoundVolume));
				break;
			}
			case CSoundObj::eWallSkid:
			{
				//Calculate skid intensity
				float intensity = 0.0;
				for(unsigned int j=0; j < o->m_AllCollisions.size(); j++)
				{
					const CCollisionData &coll = o->m_AllCollisions[j];
					float v = coll.getTangVel();
					//printf("Collision %f\n", v);
					if(0.02*v > intensity) intensity = 0.02*v; //max @ 50 m/s
				}
				if(intensity > 1.0) intensity = 1.0;

				//printf("Intensity %f\n", intensity);
				chn->setVolume(int(intensity * m_SoundVolume));
				break;
			}
			case CSoundObj::eCrash:
			case CSoundObj::eFatalCrash:
				if(o->m_AllCollisions.size() != 0)
				{
					//Calculate crash intensity
					float intensity = 0.0;
					bool fatal = false;
					for(unsigned int j=0; j < o->m_AllCollisions.size(); j++)
					{
						const CCollisionData &coll = o->m_AllCollisions[j];
						float v = coll.getRadVel();
						if(0.2*v > intensity) intensity = 0.2*v; //max @ 5 m/s

						if(coll.fatal)
							fatal = true;
					}
	
					if(chn->m_SoundType == CSoundObj::eCrash)
					{
						if(intensity > 1.0) intensity = 1.0;
						float newVolume = 1.0*intensity;

						float oldVolume = float(chn->getVolume()) / float(m_SoundVolume);
						float playTime = chn->getPlayTime();
						oldVolume -= 0.5*playTime; //assumes fade-out in 2 sec.
		
						if(newVolume > oldVolume)
						{
							chn->setVolume(int(newVolume*m_SoundVolume));
							chn->playOnce();
						}
					}

					if(fatal && chn->m_SoundType == CSoundObj::eFatalCrash)
					{
						chn->setVolume(m_SoundVolume);
						chn->playOnce();
					}
				}
				break;
			}
		}
	}

#ifdef HAVE_LIBFMOD
	//Update:
	FSOUND_Update();
#endif
}
