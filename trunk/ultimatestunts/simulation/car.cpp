/***************************************************************************
                          car.cpp  -  A car, being a moving object
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

#include "car.h"
#include "carinput.h"
CCar::CCar()
{
	m_InputData = new CCarInput;

	CBody body, wheel1, wheel2, wheel3, wheel4;
	body.m_Body = 0;
	wheel1.m_Body = 0;
	wheel2.m_Body = 0;
	wheel3.m_Body = 0;
	wheel4.m_Body = 0;

	m_WheelRPM =  m_rotationAngle = 0.0;
	m_wheelHeight1 = m_wheelHeight2 = m_wheelHeight3 = m_wheelHeight4 = 0.0;

	m_Bodies.push_back(body);
	m_Bodies.push_back(wheel1);
	m_Bodies.push_back(wheel2);
	m_Bodies.push_back(wheel3);
	m_Bodies.push_back(wheel4);

	updateBodyData();
}

CCar::~CCar(){
}

void CCar::updateBodyData()
{
	float x = 0.8;
	float y = -0.45;
	float zfr = -1.35;
	float zba = 1.45;

	float steeringangle = 0.5 * ((CCarInput *)m_InputData)->m_Right;

	m_Bodies[1].m_Position = CVector(-x, y - m_wheelHeight1, zfr);	//Left front
	m_Bodies[1].m_Orientation.rotY(steeringangle);
	m_Bodies[2].m_Position = CVector(x, y - m_wheelHeight2, zfr);	//Right front
	m_Bodies[2].m_Orientation.rotY(3.1416 + steeringangle);
	m_Bodies[3].m_Position = CVector(-x, y - m_wheelHeight3, zba);	//Left back
	m_Bodies[3].m_Orientation.rotY(0.0);
	m_Bodies[4].m_Position = CVector(x, y - m_wheelHeight4, zba);	//Right back
	m_Bodies[4].m_Orientation.rotY(3.1416);
}

void CCar::simulate(CPhysics &theSimulator)
{
	; //dummy implementation
	//TODO: something useful
}
