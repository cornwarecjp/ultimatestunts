/***************************************************************************
                          movingobject.cpp  -  Moving object class
                             -------------------
    begin                : Wed Dec 4 2002
    copyright            : (C) 2002 by CJP
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

#include "movingobject.h"
#include "movobjinput.h"
#include "physics.h"

#define DBLPI 6.283185307

CMovingObject::CMovingObject()
{
	m_InputData = new CMovObjInput;
}

CMovingObject::~CMovingObject()
{
	delete m_InputData; //I guess this will happen for all CMovingObject-derived classes

	for(unsigned int i=0; i < m_Bodies.size(); i++)
		m_Bodies[i].destroyODE();
}

/*
void CMovingObject::rememberCurrentState()
{
	m_PreviousOrientationMatrix = m_OrientationMatrix;
	m_PreviousPosition = m_Position;
}

void CMovingObject::setOrientation(CVector v)
{
	//Prevent m_OrientationVector from growing too large
	float vabs = v.abs();
	if(vabs < DBLPI)
		{m_Orientation = v;}
	else
	{
		float vabsmod = vabs - DBLPI * (int)(vabs / DBLPI);
		m_Orientation = v * (vabsmod / vabs);
	}

	m_OrientationMatrix.setRotation(m_Orientation);
	updateActualInvMomentInertia();
}

void CMovingObject::setOrientationMatrix(const CMatrix &M)
{
	m_OrientationMatrix = M;
	//TODO: something to set m_Orientation
	updateActualInvMomentInertia();
}

void CMovingObject::updateActualInvMomentInertia()
{
	m_ActualInvMomentInertia = m_OrientationMatrix * m_InvMomentInertia * m_OrientationMatrix.transpose();
}
*/

void CMovingObject::update(CPhysics *simulator, float dt)
{
	//Standard integrator
	//improved euler method:
	//x += v*dt + a*dt*dt
	//I got the idea from Racer (www.racer.nl)

	/*
	CVector Ftot, Mtot;

	getAllForces(simulator, Ftot, Mtot);

	setPosition   (m_Position    + dt* m_InvMass               *(m_Momentum        + 0.5*dt*Ftot) );
	setOrientation(m_Orientation - dt*(m_ActualInvMomentInertia*(m_AngularMomentum + 0.5*dt*Mtot)));

	m_Momentum += dt * Ftot;
	m_AngularMomentum += dt * Mtot;
	*/
}

#define g 9.81

void CMovingObject::getForces(CVector &Ftot, CVector &Mtot)
{
	/*
	I know that this defies the law of gravity, but,
	you see, I never studied law.

	Bugs Bunny
	*/

	/*
	//Standard gravity
	Ftot += CVector(0, -g/m_InvMass, 0);

	//Brake + friction
	CVector v = getVelocity();
	float vabs = v.abs();
	if(vabs < 0.001)
		{Ftot -= v;}
	else
		{Ftot -= v*cwA*vabs;}

	//rotational air friction
	CVector w = getAngularVelocity();
	float wabs = w.abs();
	if(wabs < 0.001)
		{Mtot -= w;}
	else
		{Mtot -= w*50.0*cwA*wabs;} //just guessing...
	*/
}

/*
void CMovingObject::getAllForces(CPhysics *simulator, CVector &Ftot, CVector &Mtot)
{
	getForces(Ftot, Mtot);
	simulator->addContactForces(0, Ftot, Mtot);
}
*/

CBinBuffer &CMovingObject::getData(CBinBuffer &b) const
{
	//b.addVector32(m_Position, 0.001);
	//b.addVector16(m_Momentum, 50.0);
	//b.addVector16(m_Orientation, 0.0001);
	//b.addVector16(m_AngularMomentum, 15.0);
	return b;
}

bool CMovingObject::setData(const CBinBuffer &b)
{
	//unsigned int pos = 0;
	//setPosition(b.getVector32(pos, 0.001));
	//m_Momentum = b.getVector16(pos, 50.0);
	//setOrientation(b.getVector16(pos, 0.0001));
	//m_AngularMomentum = b.getVector16(pos, 15.0);
	return true;
}
