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

#include "movingobject.h"
#include "movobjinput.h"

#define DBLPI 6.283185307

CMovingObject::CMovingObject()
{
	m_InputData = new CMovObjInput;
}

CMovingObject::~CMovingObject()
{
	delete m_InputData; //I guess this will happen for all CMovingObject-derived classes
}

void CMovingObject::rememberCurrentState()
{
	m_PreviousOrientation = m_Orientation;
	m_PreviousPosition = m_Position;
}

void CMovingObject::setOrientationVector(CVector v)
{
	//Prevent m_OrientationVector from growing too large
	float vabs = v.abs();
	if(vabs < DBLPI)
		{m_OrientationVector = v;}
	else
	{
		float vabsmod = vabs - DBLPI * (int)(vabs / DBLPI);
		m_OrientationVector = v * (vabsmod / vabs);
	}

	m_Orientation.setRotation(m_OrientationVector);
	updateActualInvMomentInertia();
}

void CMovingObject::setOrientation(const CMatrix &M)
{
	m_Orientation = M;
	//TODO: something to set m_OrientationVector
	updateActualInvMomentInertia();
}

void CMovingObject::updateActualInvMomentInertia()
{
	m_ActualInvMomentInertia = m_Orientation * m_InvMomentInertia * m_Orientation.transpose();
}

void CMovingObject::simulate(CPhysics &theSimulator)
{
	; //dummy implementation
	//TODO: something useful
}
