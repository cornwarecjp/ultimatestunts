/***************************************************************************
                          soundobj.cpp  -  description
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

#include "soundobj.h"

#ifdef HAVE_LIBFMOD

#ifdef FMOD_HEADER
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#endif

CSoundObj::CSoundObj()
{
  m_Pos.x = m_Pos.y = m_Pos.z = 0;
  m_Vel.x = m_Vel.y = m_Vel.z = 0;
}

CSoundObj::~CSoundObj()
{
	FSOUND_StopSound(m_Channel);
}

int CSoundObj::setSample(CSndSample *s)
{
	m_Channel = s->attachToChannel(FSOUND_FREE);
	FSOUND_SetPaused(m_Channel, 0);
	m_OriginalFrequency = FSOUND_GetFrequency(m_Channel);

  return 0;
}

void CSoundObj::setPos(CVector p)
{
    m_Pos = p;
    float parr[] = {m_Pos.x/10, m_Pos.y/10, m_Pos.z/10};
    float varr[] = {m_Vel.x/10, m_Vel.y/10, m_Vel.z/10};
    //printf("Setting sound pos to (%f,%f,%f)\n", p.x, p.y, p.z);
    FSOUND_3D_SetAttributes(m_Channel, parr, varr);
}

void CSoundObj::setVel(CVector v)
{
  m_Vel = v;
  float parr[] = {m_Pos.x/10, m_Pos.y/10, m_Pos.z/10};
  float varr[] = {m_Vel.x/10, m_Vel.y/10, m_Vel.z/10};
  FSOUND_3D_SetAttributes(m_Channel, parr, varr);
}

void CSoundObj::setPosVel(CVector p, CVector v)
{
	m_Pos = p;
	m_Vel = v;
	float parr[] = {m_Pos.x/10, m_Pos.y/10, m_Pos.z/10};
	float varr[] = {m_Vel.x/10, m_Vel.y/10, m_Vel.z/10};
	FSOUND_3D_SetAttributes(m_Channel, parr, varr);
}

void CSoundObj::setFrequency(float f)
{
	int freq = (int)(f * m_OriginalFrequency);
	FSOUND_SetFrequency(m_Channel, freq);
}

void CSoundObj::setVolume(int v)
{
	FSOUND_SetVolume(m_Channel, v);
}

#elif defined HAVE_LIBOPENAL

#ifdef OPENAL_HEADER
#include <AL/al.h>
#endif

CSoundObj::CSoundObj()
{
	alGenSources(1, &m_Source);
	alSourcei(m_Source, AL_LOOPING, AL_TRUE);
}

CSoundObj::~CSoundObj()
{
	alSourceStop(m_Source);
	alDeleteSources(1, &m_Source);
}

void CSoundObj::setPos(CVector p)
{
	m_Pos = p;
	alSource3f(m_Source, AL_POSITION, p.x/10, p.y/10, p.z/10);
}

void CSoundObj::setVel(CVector v)
{
	m_Vel = v;
	alSource3f(m_Source, AL_VELOCITY, v.x/10, v.y/10, v.z/10);
}

void CSoundObj::setPosVel(CVector p, CVector v)
{
	setPos(p);
	setVel(v);
}

void CSoundObj::setFrequency(float f)
{
	//F***ing openAL only supports pitch from 0.5 to 2.0!!!
	if(f < 0.5) f = 0.5;
	if(f > 2.0) f = 2.0;
	alSourcef(m_Source, AL_PITCH, f);
}

void CSoundObj::setVolume(int v)
{
	alSourcef(m_Source, AL_GAIN, ((float)v) / 255.0);
}

int CSoundObj::setSample(CSndSample *s)
{
	s->attachToChannel(m_Source);
	alSourcePlay(m_Source);
	return 0;
}

#else //libfmod and libopenAL

CSoundObj::CSoundObj()
{;}

CSoundObj::~CSoundObj()
{;}

void CSoundObj::setPos(CVector p)
{;}

void CSoundObj::setVel(CVector v)
{;}

void CSoundObj::setPosVel(CVector p, CVector v)
{;}

void CSoundObj::setFrequency(float f)
{;}

void CSoundObj::setVolume(int v)
{;}

int CSoundObj::setSample(CSndSample *s)
{return 0;}

#endif //libfmod and libopenAL
