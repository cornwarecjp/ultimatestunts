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

CGameCamera::CGameCamera(const CWorld *w)
{
	m_Velocity = CVector(0,0,0);

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

void CGameCamera::swithCameraMode()
{
	switch(m_Mode)
	{
		case In:
			setCameraMode(Tracking); break;
		case Tracking:
			setCameraMode(In); break;
		default:
			setCameraMode(In);
	}
}

void CGameCamera::setTrackedObject(int id)
{m_Id = id;}

void CGameCamera::update()
{
	float dt = m_Timer.getdt();

	if(m_Id < 0) return;

	CMovingObject *to = (m_World->m_MovObjs)[m_Id]; //tracked object
	CVector tv; //target velocity
	CVector tp; //target position
	CMatrix tm; //target orientation
	bool autotarget = false; //default: use tm

	switch(m_Mode)
	{
		case In:
			//viewpoint is higher than center of mass
			tp = to->getPosition() + CVector(0.0,0.3,0.0);
			tm = to->getRotationMatrix();
			tv = to->getVelocity();
			break;
		case Tracking:
			{
				tv = to->getVelocity();
				float vabs = tv.abs();
				float z = 0.5 * vabs + 15.0;
				float y = 0.05 * vabs + 3.0;
				tp = CVector(0.0, y, z); //further away when going faster
				tp *= to->getRotationMatrix();
				tp += to->getPosition();
				autotarget = true; //point the camera to the object
				m_Reached = false; //always have "smooth" camera movement
			}
			break;
		case UserDefined:
		case Top:
		case Television:
			break;
			//TODO: all other viewpoints
	}

	if(m_Reached)
		{m_Position = tp; m_Velocity = tv;}
	else
	{
		CVector prel = m_Position - tp;
		CVector vrel = m_Velocity - tv;

		//"damper" + "spring" model
		//spring oscillation frequency: 1.0 sec.
		//damper is needed to prevent real oscillations
		//critical damping occurs at sqrt(4*FREQ*FREQ) = 2.0
#define FREQ 1.0
#define DAMPC 1.5 // a little oscillation
		CVector a = -DAMPC*vrel - FREQ*FREQ*prel;
		m_Velocity += a * dt;
		m_Position += m_Velocity * dt;

		if(prel.abs() < 1.0) m_Reached = true;
	}

	if(autotarget)
		{m_Orientation.targetZ(m_Position - to->getPosition(), true);}
	else
		{m_Orientation = tm;}
}
