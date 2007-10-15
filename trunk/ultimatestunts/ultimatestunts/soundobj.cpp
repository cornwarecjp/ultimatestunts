/***************************************************************************
                          soundobj.cpp  -  A sound channel object
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

#include <stdlib.h>

#ifdef HAVE_LIBFMOD

#ifdef FMOD_HEADER
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#endif

#endif


#ifdef HAVE_LIBOPENAL

#ifdef OPENAL_HEADER
#include <AL/al.h>
#endif

#endif

#include "lconfig.h"
#include "timer.h"

#include "soundobj.h"

CSoundObj::CSoundObj(int movingobjectID, bool looping)
{
	m_MovingObjectID = movingobjectID;
	m_Looping = looping;
	m_Pos.x = m_Pos.y = m_Pos.z = 0;
	m_Vel.x = m_Vel.y = m_Vel.z = 0;

	m_PlayStartTime = CTimer::getTime();

	m_CurrentSample = NULL;

#ifdef HAVE_LIBOPENAL
	alGenSources(1, &m_Source);
	if(looping)
		{alSourcei(m_Source, AL_LOOPING, AL_TRUE);}
	else
		{alSourcei(m_Source, AL_LOOPING, AL_FALSE);}

	alSourcef(m_Source, AL_REFERENCE_DISTANCE, 10.0);

	m_WorkaroundPitchBug =
		theMainConfig->getValue("workaround", "openal_008_pitch") == "true";
#endif
}


CSoundObj::~CSoundObj()
{
#ifdef HAVE_LIBFMOD
	FSOUND_StopSound(m_Channel);
#endif

#ifdef HAVE_LIBOPENAL
	alSourceStop(m_Source);
	alDeleteSources(1, &m_Source);
#endif
}

int CSoundObj::setSample(CSndSample *s)
{
	m_CurrentSample = s;

#ifdef HAVE_LIBFMOD
	m_Channel = s->attachToChannel(FSOUND_FREE);
	m_OriginalFrequency = FSOUND_GetFrequency(m_Channel);
	if(m_Looping)
	{
		FSOUND_SetLoopMode(m_Channel, FSOUND_LOOP_NORMAL);
		setVolume(0); //initial volume
		FSOUND_SetPaused(m_Channel, 0);
	}
	else
	{
		FSOUND_SetLoopMode(m_Channel, FSOUND_LOOP_OFF);
		FSOUND_SetPaused(m_Channel, 1);
	}

	FSOUND_3D_SetMinMaxDistance(m_Channel, 10.0, 200.0);
#endif

#ifdef HAVE_LIBOPENAL
	s->attachToChannel(m_Source);
	if(m_Looping)
	{
		setVolume(0); //initial volume
		alSourcePlay(m_Source);
	}
#endif

	return 0;
}

void CSoundObj::setPos(CVector p)
{
	m_Pos = p;
#ifdef HAVE_LIBFMOD
	float parr[] = {m_Pos.x, m_Pos.y, -m_Pos.z};
	float varr[] = {m_Vel.x, m_Vel.y, -m_Vel.z};
	//printf("Setting sound pos to (%f,%f,%f)\n", p.x, p.y, p.z);
	FSOUND_3D_SetAttributes(m_Channel, parr, varr);
#endif

#ifdef HAVE_LIBOPENAL
	alSource3f(m_Source, AL_POSITION, p.x, p.y, p.z);
#endif
}

void CSoundObj::setVel(CVector v)
{
	m_Vel = v;
#ifdef HAVE_LIBFMOD
	float parr[] = {m_Pos.x, m_Pos.y, -m_Pos.z};
	float varr[] = {m_Vel.x, m_Vel.y, -m_Vel.z};
	FSOUND_3D_SetAttributes(m_Channel, parr, varr);
#endif

#ifdef HAVE_LIBOPENAL
	alSource3f(m_Source, AL_VELOCITY, v.x, v.y, v.z);
#endif
}

void CSoundObj::setPosVel(CVector p, CVector v)
{
	m_Pos = p;
	m_Vel = v;
#ifdef HAVE_LIBFMOD
	float parr[] = {m_Pos.x, m_Pos.y, -m_Pos.z};
	float varr[] = {m_Vel.x, m_Vel.y, -m_Vel.z};
	FSOUND_3D_SetAttributes(m_Channel, parr, varr);
#endif

#ifdef HAVE_LIBOPENAL
	setPos(p);
	setVel(v);
#endif
}

void CSoundObj::setFrequency(float f)
{
#ifdef HAVE_LIBFMOD
	int freq = (int)(f * m_OriginalFrequency);
	FSOUND_SetFrequency(m_Channel, freq);
#endif

#ifdef HAVE_LIBOPENAL
	if(m_WorkaroundPitchBug)
	{
		//Workaround for OpenAL 0.0.8, which doesn't meet it's own OpenAL 1.1 specs
		if(f > 1.999) f = 1.999;
		if(f < 0.001) f = 0.001;
	}

	alSourcef(m_Source, AL_PITCH, f);
#endif
}

void CSoundObj::setVolume(int v)
{
	m_Volume = v;

#ifdef HAVE_LIBFMOD
	FSOUND_SetVolume(m_Channel, v);
#endif

#ifdef HAVE_LIBOPENAL
	alSourcef(m_Source, AL_GAIN, ((float)v) / 255.0);
#endif
}

int CSoundObj::getVolume() const
{
#ifdef HAVE_LIBOPENAL
	ALfloat volume;
	alGetSourcef(m_Source, AL_GAIN, &volume);
	return int(255 * volume);
#endif

	return m_Volume;
}

void CSoundObj::playOnce()
{
#ifdef HAVE_LIBFMOD
	if(m_CurrentSample == NULL) return;

	FSOUND_SetPaused(m_Channel, 1);
	m_CurrentSample->attachToChannel(m_Channel);
	FSOUND_SetCurrentPosition(m_Channel, 0);
	FSOUND_SetLoopMode(m_Channel, FSOUND_LOOP_OFF);
	FSOUND_SetPaused(m_Channel, 0);
	
	//It seems that the state needs to be reloaded
	setPosVel(m_Pos, m_Vel);
	FSOUND_SetVolume(m_Channel, m_Volume);
#endif

#ifdef HAVE_LIBOPENAL
	alSourcePlay(m_Source);
#endif

	m_PlayStartTime = CTimer::getTime();

	//printf("CSoundObj::playOnce() called\n");
}

float CSoundObj::getPlayTime() const
{
	return CTimer::getTime() - m_PlayStartTime;
}


