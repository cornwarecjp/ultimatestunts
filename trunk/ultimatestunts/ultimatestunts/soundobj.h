/***************************************************************************
                          soundobj.h  -  description
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

#ifndef SOUNDOBJ_H
#define SOUNDOBJ_H


/**
  *@author CJP
  */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "cstring.h"
#include "vector.h"
#include "sndsample.h"

class CSoundObj
{
public:
	CSoundObj(int movingobjectID);
	~CSoundObj();
	int setSample(CSndSample *s);

	void setPos(CVector p);
	void setVel(CVector v);
	void setPosVel(CVector p, CVector v);

	void setFrequency(float f); //1.0 = original sound
	void setVolume(int v); //0 - 255

	int getMovingObjectID()
		{return m_MovingObjectID;}

protected:
	CVector m_Pos;
	CVector m_Vel;
	int m_MovingObjectID;

#ifdef HAVE_LIBFMOD
	int m_Channel;

	int m_OriginalFrequency;
#endif

#ifdef HAVE_LIBOPENAL
	unsigned int m_Source;
#endif

};

#endif
