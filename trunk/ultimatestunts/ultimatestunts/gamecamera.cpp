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
#include <cmath>
#ifndef M_PI
#define M_PI 3.141592653
#endif

#include "gamecamera.h"
#include "usmacros.h"

CGameCamera::CGameCamera(const CWorld *w)
{
	m_Velocity = CVector(0,0,0);

	m_Mode = In;
	m_Id = -1;
	m_World = w;

	m_Reached = true;
	m_First = true;
}

CGameCamera::~CGameCamera(){
}

void CGameCamera::setCameraMode(eCameraMode mode)
{
	m_Reached = false;
	m_First = true;
	m_Mode = mode;
	m_SwitchTime = m_Timer.getTime();
}

void CGameCamera::swithCameraMode()
{
	switch(m_Mode)
	{
		case In:
			setCameraMode(Tracking); break;
		case Tracking:
			setCameraMode(Top); break;
		case Top:
			setCameraMode(Television); break;
		case Television:
			setCameraMode(In); break;
		default:
			setCameraMode(In);
	}
}

void CGameCamera::setTrackedObject(int id)
{
	m_Id = id;
	m_Reached = false;
	m_First = true;
	m_SwitchTime = m_Timer.getTime();
}

void CGameCamera::switchTrackedObject()
{
	unsigned int i = m_Id + 1;
	if(i >= m_World->getNumObjects(CDataObject::eMovingObject)) i = 0;
	setTrackedObject(i);
}

void CGameCamera::update()
{
	float dt = m_Timer.getdt();

	if(m_Id < 0) return;

	const CMovingObject *to = m_World->getMovingObject(m_Id); //tracked object
	CVector tv; //target velocity
	CVector tp; //target position
	CMatrix tm; //target orientation
	float reach_thr = 1.0;
	bool autotarget = false; //default: use tm

	switch(m_Mode)
	{
		case In:
			tp = to->m_Bodies[0].getPosition() + to->m_Bodies[0].getOrientationMatrix() * to->getCameraPos();
			if(m_Reached)
				{tm = to->m_Bodies[0].getOrientationMatrix();}
			//else: autotarget

			tv = to->m_Bodies[0].getVelocity();
			break;
		case Tracking:
			{
				tv = to->m_Bodies[0].getVelocity();
				float vabs = tv.abs();
				const CMatrix &rmat = to->m_Bodies[0].getOrientationMatrix();
				tp = CVector(rmat.Element(2,0), 0.0, rmat.Element(2,2));
				tp.normalise();
				tp *= 0.5 * vabs + 15.0; //further away when going faster
				tp.y = 0.01 * vabs + 3.0;
				tp += to->m_Bodies[0].getPosition();
				autotarget = true; //point the camera to the object
				m_Reached = false; //always have "smooth" camera movement
			}
			break;
		case Top:
			{
				reach_thr = 5.0;
				tv = to->m_Bodies[0].getVelocity();
				float vabs = tv.abs();
				tp = to->m_Bodies[0].getPosition() + CVector(0.0, 20.0 + 5.0*vabs, 0.0);
				if(m_Reached)
				{
					CMatrix m;
					m.setElement(1,1,0.0);
					m.setElement(2,2,0.0);
					m.setElement(1,2,-1.0);
					m.setElement(2,1,1.0);
					tm = m;
				}
				//else: autotarget
			}
			break;
		case Television:
			{
				tp = to->m_Bodies[0].getPosition();
				int x = (int)tp.x;
				int y = (int)tp.y;
				int z = (int)tp.z;
				x = x - x % (2*TILESIZE) + (int)(1.25*TILESIZE);
				y = y - y % (VERTSIZE) + VERTSIZE;
				z = z - z % (2*TILESIZE) + (int)(1.25*TILESIZE);
				tp.x = (float)x;
				tp.y = (float)y;
				tp.z = (float)z;
				m_Reached = true; //immediately
				autotarget = true;
			}
			break;
		case UserDefined:
			break;
			//TODO: all other viewpoints
	}

	if(m_Reached)
		{m_Position = tp; m_Velocity = tv;}
	else
	{
		autotarget = true; //always autotarget when not reached

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

		if(prel.abs() < reach_thr || (m_Timer.getTime() - m_SwitchTime) > 2.0)
			m_Reached = true;
	}

	if(autotarget)
		{m_Orientation.targetZ(m_Position - to->m_Bodies[0].getPosition(), true);}
	else
		{m_Orientation = tm;}
}
