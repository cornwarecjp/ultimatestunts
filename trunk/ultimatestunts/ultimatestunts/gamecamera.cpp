/***************************************************************************
                          gamecamera.cpp  -  The camera being used in the game
                             -------------------
    begin                : ma feb 3 2003
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
#include <math.h>

#include "gamecamera.h"

#define NEWPERFRAME 0.1
#define OLDPERFRAME (1-NEWPERFRAME)

CGameCamera::CGameCamera(const CWorld *w)
{
	m_Mode = In;
	m_Id = -1;
	m_World = w;

	m_Reached = true;
}

CGameCamera::~CGameCamera(){
}

void CGameCamera::setCameraMode(eCameraMode mode)
{
	m_Reached = false;
	m_Mode = mode;
}

void CGameCamera::setTrackedObject(int id)
{m_Id = id;}

void CGameCamera::update()
{
	if(m_Id < 0) return;

	CMovingObject *to = (m_World->m_MovObjs)[m_Id]; //tracked object
	CVector tp; //target position
	CMatrix tm; //target orientation

	switch(m_Mode)
	{
		case In:
			tp = to->getPosition();
			tm = to->getRotationMatrix();
			break;
		case Tracking:
		case UserDefined:
		case Top:
		case Television:
			break;
			//TODO: all other viewpoints
	}

	if(m_Reached)
		{m_Position = tp;}
	else
	{
		m_Position = OLDPERFRAME * m_Position + NEWPERFRAME * tp;
		if((m_Position - tp).abs() < 1.0) m_Reached = true;
	}

	m_Orientation = tm; //TODO: better rotation

}
