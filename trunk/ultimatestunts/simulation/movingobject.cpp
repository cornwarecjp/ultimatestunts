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

CMovingObject::CMovingObject()
{
	m_InputData = new CMovObjInput;

	//Give some default values
	//Just for not letting them uninitialised
	//The simulation should re-initialise them.
	m_Position = m_Velocity = m_Orientation = m_AngularVelocity = CVector(0,0,0);

	m_Position = CVector(400.0,0.8,450.0); //TODO: put this somewhere else

	m_RotationMatrix = CMatrix(m_Orientation);
}

CMovingObject::~CMovingObject()
{
	delete m_InputData; //I guess this will happen for all CMovingObject-derived classes
}

void CMovingObject::setOrientation(CVector v)
{
	m_Orientation = v;
	m_RotationMatrix = CMatrix(m_Orientation);
}

void CMovingObject::setRotationMatrix(CMatrix &M)
{
	m_RotationMatrix = M;
	//and something to set m_Orientation
}

void CMovingObject::simulate(CPhysics &theSimulator)
{
	; //dummy implementation
	//TODO: something useful
}
