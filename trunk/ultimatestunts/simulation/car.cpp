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
	m_Position = CVector(400.0,0.8,450.0); //TODO: put this somewhere else

	m_InputData = new CCarInput;

	//Five bodies:
	CBody body, wheel1, wheel2, wheel3, wheel4;
	body.m_Body = 0;
	wheel1.m_Body = 0;
	wheel2.m_Body = 0;
	wheel3.m_Body = 0;
	wheel4.m_Body = 0;

	//One sound:
	m_Sounds.push_back(0);

	m_FrontWheelNeutral = CVector(0.8, -0.4, -1.75);
	m_BackWheelNeutral = CVector(0.8, -0.4, 1.1);
	m_Mass = 1000.0; //kilogram
	m_WheelRadius = 0.35; //meter

	m_WheelVelocity =  m_WheelAngle = 0.0;
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

void CCar::simulate(CPhysics &theSimulator)
{
	; //dummy implementation
	//TODO: something useful
}

void CCar::updateBodyData()
{
	CMatrix wheelmatrix;
	wheelmatrix.rotX(m_WheelAngle);

#define xfr	(m_FrontWheelNeutral.x)
#define xba	(m_BackWheelNeutral.x)
#define yfr	(m_FrontWheelNeutral.y)
#define yba	(m_BackWheelNeutral.y)
#define zfr	(m_FrontWheelNeutral.z)
#define zba	(m_BackWheelNeutral.z)

	float steeringangle = 0.5 * ((CCarInput *)m_InputData)->m_Right;

	CMatrix &leftm = m_Bodies[1].m_Orientation;
	CMatrix &rightm = m_Bodies[2].m_Orientation;

	leftm.rotY(steeringangle);
	rightm.rotY(3.1416 + steeringangle);

	leftm = wheelmatrix * leftm;
	rightm = wheelmatrix * rightm;

	CMatrix flipRight;
	flipRight.rotY(3.1416); //TODO: faster CMatrix method

	m_Bodies[1].m_Position = CVector(-xfr, yfr - m_wheelHeight1, zfr);	//Left front
	m_Bodies[2].m_Position = CVector(xfr, yfr - m_wheelHeight2, zfr);	//Right front
	m_Bodies[3].m_Position = CVector(-xba, yba - m_wheelHeight3, zba);	//Left back
	m_Bodies[3].m_Orientation = wheelmatrix;
	m_Bodies[4].m_Position = CVector(xba, yba - m_wheelHeight4, zba);	//Right back
	m_Bodies[4].m_Orientation = wheelmatrix * flipRight;
}
