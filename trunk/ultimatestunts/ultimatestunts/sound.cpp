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
#include <stdlib.h>
#include <math.h>
#include <stdio.h> //debugging

#include "sound.h"

#include "vector.h"
#include "usmacros.h"
#include "datafile.h"

#ifdef HAVE_LIBFMOD

#ifdef FMOD_HEADER
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#include <fmod/wincompat.h> //debugging
#endif

CSound::CSound(const CLConfig &conf, const CWorld *world)
{
	m_World = world;

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

/*
			{
				unsigned int caps = 0;

				FSOUND_GetDriverCaps(i, &caps);
				if (caps & FSOUND_CAPS_HARDWARE)
					printf("     * Driver supports hardware 3D sound!\n");
				if (caps & FSOUND_CAPS_EAX2)
					printf("     * Driver supports EAX 2 reverb!\n");
				if (caps & FSOUND_CAPS_EAX3)
					printf("     * Driver supports EAX 3 reverb!\n");
				if (caps & FSOUND_CAPS_GEOMETRY_OCCLUSIONS)
					printf("     * Driver supports hardware 3d geometry processing with occlusions!\n");
				if (caps & FSOUND_CAPS_GEOMETRY_REFLECTIONS)
					printf("     * Driver supports hardware 3d geometry processing with reflections!\n");
			}
*/

		}
	}

	printf("   Loaded driver: %s\n", FSOUND_GetDriverName(FSOUND_GetDriver()) );

/*
  {
    unsigned int caps = 0;

    FSOUND_GetDriverCaps(FSOUND_GetDriver(), &caps);
    printf("---------------------------------------------------------\n");
    printf("Driver capabilities\n");
    printf("---------------------------------------------------------\n");
    if (!caps)
      printf("- This driver will support software mode only.\n  It does not properly support 3D sound hardware.\n");
    if (caps & FSOUND_CAPS_HARDWARE)
      printf("- Driver supports hardware 3D sound!\n");
    if (caps & FSOUND_CAPS_EAX2)
      printf("- Driver supports EAX 2 reverb!\n");
    if (caps & FSOUND_CAPS_EAX3)
      printf("- Driver supports EAX 3 reverb!\n");
    if (caps & FSOUND_CAPS_GEOMETRY_OCCLUSIONS)
      printf("- Driver supports hardware 3d geometry processing with occlusions!\n");
    if (caps & FSOUND_CAPS_GEOMETRY_REFLECTIONS)
      printf("- Driver supports hardware 3d geometry processing with reflections!\n");
    printf("---------------------------------------------------------\n");
  }
*/

  // INITIALIZE
  if (!FSOUND_Init(44100, 32, 0))
  {
    printf("  Init: %s\n", FMOD_ErrorString(FSOUND_GetError()));
    return;
  }

/*
  // DISPLAY HELP
  printf("FMOD Output Method : ");
  switch (FSOUND_GetOutput())
  {
    case FSOUND_OUTPUT_NOSOUND:
      printf("nosound\n");
      break;
#ifdef __CYGWIN__
    case FSOUND_OUTPUT_WINMM:
      printf("winmm\n");
      break;
    case FSOUND_OUTPUT_DSOUND:
      printf("directsound\n");
      break;
    case FSOUND_OUTPUT_A3D:
      printf("a3d\n");
      break;
#elif defined(__linux__)
    case FSOUND_OUTPUT_OSS:
      printf("oss\n");
      break;
    case FSOUND_OUTPUT_ESD:
      printf("esd\n");
      break;
    case FSOUND_OUTPUT_ALSA:
      printf("alsa\n");
      break;
#endif
  };

  printf("FMOD Mixer         : ");
  switch (FSOUND_GetMixer())
  {
    case FSOUND_MIXER_BLENDMODE:
      printf("FSOUND_MIXER_BLENDMODE\n");
      break;
    case FSOUND_MIXER_MMXP5:
      printf("FSOUND_MIXER_MMXP5\n");
      break;
    case FSOUND_MIXER_MMXP6:
      printf("FSOUND_MIXER_MMXP6\n");
      break;
    case FSOUND_MIXER_QUALITY_FPU:
      printf("FSOUND_MIXER_QUALITY_FPU\n");
      break;
    case FSOUND_MIXER_QUALITY_MMXP5:
      printf("FSOUND_MIXER_QUALITY_MMXP5\n");
      break;
    case FSOUND_MIXER_QUALITY_MMXP6:
      printf("FSOUND_MIXER_QUALITY_MMXP6\n");
      break;
  };

  printf("Sub driver        : ");
  printf("%s\n", FSOUND_GetDriverName(FSOUND_GetDriver()));
  printf("Hardware 3D channels : %d\n", FSOUND_GetNumHardwareChannels());
*/

	CDataFile f("music/sv.ogg");
	printf("   Loading music file %s\n", f.getName().c_str());
	m_MusicSample = new CSndSample(SAMPLE_2D);
	m_MusicObject = new CSoundObj;
	m_MusicSample->loadFromFile(f.useExtern());
	m_MusicObject->setSample(m_MusicSample);

	m_MusicVolume = conf.getValue("sound", "musicvolume").toInt();
	m_SoundVolume = conf.getValue("sound", "soundvolume").toInt();

	m_MusicObject->setVolume(m_MusicVolume);
}

CSound::~CSound()
{
	unload();

	delete m_MusicObject;
	delete m_MusicSample;

	FSOUND_Close();
}

bool CSound::load()
{
	CSndSample *engine = new CSndSample(SAMPLE_3D);
	CDataFile f("sounds/engine.wav");
	printf("   Loading enine sound from %s\n", f.getName().c_str());
	engine->loadFromFile(f.useExtern());

	m_Samples.push_back(engine);

	for(unsigned int i=0; i<m_World->m_MovObjs.size(); i++)
		if(m_World->m_MovObjs[i]->getType() == CMessageBuffer::car)
		{
			CSoundObj *o = new CSoundObj;
			o->setSample(engine);
			o->setPosVel(CVector(0,0,0), CVector(0,0,0));

			m_Channels.push_back(o);
			m_ObjIDs.push_back(i);
		}

	return true;
}

void CSound::unload()
{
	for(unsigned int i=0; i<m_Channels.size(); i++)
		delete m_Channels[i];

	for(unsigned int i=0; i<m_Samples.size(); i++)
		delete m_Samples[i];

	m_Channels.clear();
	m_Samples.clear();
	m_ObjIDs.clear();
}

void CSound::update()
{
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
	for(unsigned int i=0; i<m_Channels.size(); i++)
	{
		CMovingObject *o = m_World->m_MovObjs[m_ObjIDs[i]];
		CVector v = o->getVelocity();
		m_Channels[i]->setPosVel(o->getPosition(), v);
		int vol = 20 + (int)(4*v.abs());
		float freq = 0.1 + 0.1 * v.abs();
		//printf("Setting vol,freq of %d to %d,%3.3f\n", i, vol, freq);
		m_Channels[i]->setFrequency(freq);
		m_Channels[i]->setVolume((vol * m_SoundVolume) >> 8);
	}

	//Update:
	FSOUND_3D_Update();
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

void CSound::update()
{;}
#endif //libfmod
