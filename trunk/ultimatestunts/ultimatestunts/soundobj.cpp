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
#include <stdio.h> //debugging
#include <stdlib.h>

#include "soundobj.h"

#ifdef HAVE_LIBFMOD

#ifdef FMOD_HEADER
#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>
#include <fmod/wincompat.h> //debugging
#endif

CSoundObj::CSoundObj()
{
  //m_Sample = NULL;
  m_Pos.x = m_Pos.y = m_Pos.z = 0;
  m_Vel.x = m_Vel.y = m_Vel.z = 0;
}

CSoundObj::~CSoundObj()
{
  //FSOUND_Sample_Free((FSOUND_SAMPLE  *)m_Sample);
  //m_Sample = NULL;
}

int CSoundObj::setSample(CSndSample *s)
{
  m_Channel = s->attachToChannel(FSOUND_FREE);

  FSOUND_SetPaused(m_Channel, FALSE);

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

#else //no libfmod
CSoundObj::CSoundObj()
{;}

CSoundObj::~CSoundObj()
{;}

void CSoundobj::setPos(CVector p)
{;}

void CSoundobj::setVel(CVector v)
{;}

int CSoundobj::setSample(CSndSample *s)
{;}

#endif //libfmod
