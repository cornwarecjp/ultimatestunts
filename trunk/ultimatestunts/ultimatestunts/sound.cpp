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

#ifdef HAVE_LIBFMOD

#ifdef FMOD_HEADER
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#include <fmod/wincompat.h> //debugging
#endif

CSound::CSound(const CLConfig &conf, const CWorld *world)
{
	printf("sound::sound()\n");

	m_World = world;

	if (FSOUND_GetVersion() < FMOD_VERSION)
	{
		printf("Error : You are using the wrong DLL version!  You should be using FMOD %.02f\n", FMOD_VERSION);
		return;
	}
  // ==========================================================================================
  // SELECT OUTPUT METHOD
  // ==========================================================================================
  printf("---------------------------------------------------------\n");
  printf("Output Type\n");
  printf("---------------------------------------------------------\n");

#ifdef __CYGWIN__
  printf("1 - Direct Sound\n");
  printf("2 - Windows Multimedia Waveout\n");
  printf("3 - A3D\n");
#elif defined(__linux__)
  printf("1 - Open Sound System (OSS) (Linux, Solaris, freebsd)\n");
  printf("2 - Elightment Sound Daemon (ESD, Linux, more ...)\n");
  printf("3 - Alsa Sound System (Linux)\n");
#endif
  printf("4 - NoSound\n");
  printf("---------------------------------------------------------\n");
// print driver names
  printf("Press a corresponding number\n");

  char key;
  do
  {
    key = getchar();
  } while (key < '1' || key > '4');

  switch (key)
  {
#ifdef __CYGWIN__
    case '1' :      FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
      break;
    case '2' :      FSOUND_SetOutput(FSOUND_OUTPUT_WINMM);
      break;
    case '3' :      FSOUND_SetOutput(FSOUND_OUTPUT_A3D);
      break;
#elif defined(__linux__)
    case '1' :  FSOUND_SetOutput(FSOUND_OUTPUT_OSS);
      break;
    case '2' :  FSOUND_SetOutput(FSOUND_OUTPUT_ESD);
      break;
    case '3' :  FSOUND_SetOutput(FSOUND_OUTPUT_ALSA);
      break;
#endif
    default:
    case '4' :      FSOUND_SetOutput(FSOUND_OUTPUT_NOSOUND);
      printf("No sound will be available\n");
      break;
  }
  // ==========================================================================================
  // SELECT DRIVER
  // ==========================================================================================

  // The following list are the drivers for the output method selected above.
  printf("---------------------------------------------------------\n");

  switch (FSOUND_GetOutput())
  {
    case FSOUND_OUTPUT_NOSOUND:
      printf("NoSound");
      break;
#ifdef __CYGWIN__
    case FSOUND_OUTPUT_WINMM:
      printf("Windows Multimedia Waveout");
      break;
    case FSOUND_OUTPUT_DSOUND:
      printf("Direct Sound");
      break;
    case FSOUND_OUTPUT_A3D:
      printf("A3D");
      break;
#elif defined(__linux__)
    case FSOUND_OUTPUT_OSS:
      printf("Open Sound System");
      break;
    case FSOUND_OUTPUT_ESD:
      printf("Enlightment Sound Daemon");
      break;
    case FSOUND_OUTPUT_ALSA:
      printf("Alsa");
      break;
#endif
  };
  printf(" Driver list\n");
  printf("---------------------------------------------------------\n");

  for (int i=0; i < FSOUND_GetNumDrivers(); i++)
  {
    printf("%d - %s\n", i+1, FSOUND_GetDriverName(i));      // print driver names
    {
      unsigned int caps = 0;

      FSOUND_GetDriverCaps(i, &caps);
      if (caps & FSOUND_CAPS_HARDWARE)
        printf("  * Driver supports hardware 3D sound!\n");
      if (caps & FSOUND_CAPS_EAX2)
        printf("  * Driver supports EAX 2 reverb!\n");
      if (caps & FSOUND_CAPS_EAX3)
        printf("  * Driver supports EAX 3 reverb!\n");
      if (caps & FSOUND_CAPS_GEOMETRY_OCCLUSIONS)
        printf("  * Driver supports hardware 3d geometry processing with occlusions!\n");
      if (caps & FSOUND_CAPS_GEOMETRY_REFLECTIONS)
        printf("  * Driver supports hardware 3d geometry processing with reflections!\n");
    }
  }

  printf("---------------------------------------------------------\n");

	// print driver names
	printf("Press a corresponding number\n");
	m_Driver = 0;
	do
	{
		key = getchar() - '1';
	}
	while (key < 0 || key >= FSOUND_GetNumDrivers());

	m_Driver = key;
	FSOUND_SetDriver(m_Driver);		// Select sound card (0 = default)
	printf("Selected driver: %d\n", m_Driver + 1);
	printf("Loaded driver: %d\n", FSOUND_GetDriver() + 1);

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

  // ==========================================================================================
  // INITIALIZE
  // ==========================================================================================
  if (!FSOUND_Init(44100, 32, 0))
  {
    printf("Init: %s\n", FMOD_ErrorString(FSOUND_GetError()));
    return;
  }

  // ==========================================================================================
  // DISPLAY HELP
  // ==========================================================================================

  printf("FSOUND Output Method : ");
  switch (FSOUND_GetOutput())
  {
    case FSOUND_OUTPUT_NOSOUND:
      printf("FSOUND_OUTPUT_NOSOUND\n");
      break;
#ifdef __CYGWIN__
    case FSOUND_OUTPUT_WINMM:
      printf("FSOUND_OUTPUT_WINMM\n");
      break;
    case FSOUND_OUTPUT_DSOUND:
      printf("FSOUND_OUTPUT_DSOUND\n");
      break;
    case FSOUND_OUTPUT_A3D:
      printf("FSOUND_OUTPUT_A3D\n");
      break;
#elif defined(__linux__)
    case FSOUND_OUTPUT_OSS:
      printf("Open Sound System");
      break;
    case FSOUND_OUTPUT_ESD:
      printf("Enlightment Sound Daemon");
      break;
    case FSOUND_OUTPUT_ALSA:
      printf("Alsa");
      break;
#endif
  };

  printf("FSOUND Mixer         : ");
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
  printf("FSOUND Driver        : ");
  printf("%s\n", FSOUND_GetDriverName(FSOUND_GetDriver()));
  printf("Hardware 3D channels : %d\n", FSOUND_GetNumHardwareChannels());

	m_TopDir = conf.getValue("files", "datadir");
	CString fn = m_TopDir + "music/canyon.mp3";
	printf("\nLoading music file %s\n", fn.c_str());
	m_MusicSample = new CSndSample(SAMPLE_2D);
	m_MusicObject = new CSoundObj;
	m_MusicSample->loadFromFile(fn);
	m_MusicObject->setSample(m_MusicSample);
	m_MusicObject->setVolume(120); //less than 50% volume
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
	engine->loadFromFile(m_TopDir + "sounds/engine.wav");

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
		m_Channels[i]->setVolume(vol);
	}

	//Update:
	FSOUND_3D_Update();
}

#else

CSound::CSound(const CLConfig &conf, const CWorld *world)
{
	printf("No sound: sound support not compiled in\n");
}

CSound::~CSound()
{;}

bool CSound::load()
{return true;}

void CSound::update()
{;}
#endif //libfmod
