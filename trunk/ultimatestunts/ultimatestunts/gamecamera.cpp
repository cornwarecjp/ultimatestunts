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

#include <cstdio>
 
#include <cmath>
#include "pi.h"

#include "gamecamera.h"
#include "usmacros.h"
#include "world.h"

CGameCamera::CGameCamera()
{
	m_Velocity = CVector(0,0,0);

	m_Mode = In;
	m_Id = -1;

	m_PrimaryTarget = 0;

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
	if(i >= theWorld->getNumObjects(CDataObject::eMovingObject)) i = 0;
	setTrackedObject(i);
}

void CGameCamera::update()
{
	//float dt = m_Timer.getdt();
	float dt = theWorld->m_Lastdt;

	if(m_Id < 0) return;

	const CMovingObject *to = theWorld->getMovingObject(m_Id); //tracked object
	CVector tv; //target velocity
	CVector tp; //target position
	CMatrix tm; //target orientation
	float reach_thr = 1.0;
	bool autotarget = false; //default: use tm
	CVector autotargetOffset; //default: 0,0,0

	switch(m_Mode)
	{
		case In:
			tp = to->m_Position + to->m_OrientationMatrix * to->getCameraPos();
			if(m_Reached)
			{
				tm.rotX(0.1);
				tm *= to->m_OrientationMatrix;
			}
			//else: autotarget

			tv = to->m_Velocity;
			break;
		case Tracking:
			{
				tv = to->m_Velocity;
				float vabs = tv.abs();
				const CMatrix &rmat = to->m_OrientationMatrix;
				tp = CVector(rmat.Element(2,0), 0.0, rmat.Element(2,2));
				tp.normalise();
				tp *= (0.1 * vabs + 10.0); //further away when going faster
				tp.y += (0.02 * vabs + 2.0);
				tp += to->m_Position;
				autotargetOffset = CVector(0,1.5,0);
				autotarget = true; //point the camera to the object
				m_Reached = false; //always have "smooth" camera movement
			}
			break;
		case Top:
			{
				reach_thr = 5.0;
				tv = to->m_Velocity;
				float vabs = tv.abs();
				tp = CVector(tv.x, 0.0, tv.z);
				tp.normalise();
				tp *= 0.25 * vabs; //further away when going faster
				tp += CVector(0.0, 20.0 + 1.0*vabs, 0.0); //higher when going faster
				tp += to->m_Position;
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
				tp = to->m_Position;
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

	//Set the initial position:
	if(m_Position.abs2() < 0.01)
		m_Reached = true;

	if(m_Reached)
		{m_Position = tp; m_Velocity = tv;}
	else
	{
		autotarget = true; //always autotarget when not reached

		CVector prel = m_Position - tp;
		CVector vrel = m_Velocity - tv;

		//"damper" + "spring" model
		//damper is needed to prevent real oscillations
		//spring oscillation frequency is sqrt(SPRC) / (2*M_PI)
		//critical damping occurs at DAMPC = sqrt(4*SPRC)
#define SPRC  15.7 //frequency is 2.5 Hz
#define DAMPC (sqrt(4*SPRC)) //critical damping

		//we use the analytical solution to compensate for low framerates
		//find the 3D solution x(t) = A * exp(-lambda*t) * cos(omega*t + phi)
		//where A and phi can be different per coordinate
		float omega = sqrt(SPRC);
		float lambda = DAMPC/2;

		CVector Acosphi = prel;
		CVector Asinphi = (lambda * prel + vrel) / omega;
		CVector A(
			sqrt(Acosphi.x*Acosphi.x + Asinphi.x*Asinphi.x),
			sqrt(Acosphi.y*Acosphi.y + Asinphi.y*Asinphi.y),
			sqrt(Acosphi.z*Acosphi.z + Asinphi.z*Asinphi.z)
			);
		CVector phi(
			atan2f(Asinphi.x / A.x,Acosphi.x / A.x),
			atan2f(Asinphi.y / A.y,Acosphi.y / A.y),
			atan2f(Asinphi.z / A.z,Acosphi.z / A.z)
			);

		//correct for division by zero
		if(fabsf(A.x) < 0.001) phi.x = 0.0;
		if(fabsf(A.y) < 0.001) phi.y = 0.0;
		if(fabsf(A.z) < 0.001) phi.z = 0.0;

		CVector prelnew(
			A.x * exp(-lambda*dt) * cos(omega*dt+phi.x),
			A.y * exp(-lambda*dt) * cos(omega*dt+phi.y),
			A.z * exp(-lambda*dt) * cos(omega*dt+phi.z)
			);

		CVector vrelnew(
			A.x * exp(-lambda*dt) * (-lambda*cos(omega*dt+phi.x) + omega*sin(omega*dt+phi.x)),
			A.y * exp(-lambda*dt) * (-lambda*cos(omega*dt+phi.y) + omega*sin(omega*dt+phi.y)),
			A.z * exp(-lambda*dt) * (-lambda*cos(omega*dt+phi.z) + omega*sin(omega*dt+phi.z))
			);

		//fprintf(stderr, "prel = %s phi = %s\n", CString(prel).c_str(), CString(phi).c_str());
		//CVector a = -DAMPC*vrel - SPRC*prel;
		m_Velocity += (vrelnew - vrel); //a * dt;
		m_Position += (prelnew - prel); //m_Velocity * dt;

		if(prel.abs() < reach_thr || (m_Timer.getTime() - m_SwitchTime) > 2.0)
			m_Reached = true;
	}

	if(autotarget)
		{m_Orientation.targetZ(m_Position - to->m_Position - autotargetOffset, true);}
	else
		{m_Orientation = tm;}
}
