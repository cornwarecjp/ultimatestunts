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

	//Five bodies:
	CBody body, wheel1, wheel2, wheel3, wheel4;
	body.m_Body = 0;
	wheel1.m_Body = 0;
	wheel2.m_Body = 0;
	wheel3.m_Body = 0;
	wheel4.m_Body = 0;

	//One sound:
	m_Sounds.push_back(0);

	cwA = 10.0; //air friction
	
	m_FrontWheelNeutral = CVector(0.8, -0.4, -1.75);
	m_BackWheelNeutral = CVector(0.8, -0.4, 1.1);
	m_InvMass = 0.001; //kilogram^-1

	//Just guessing the size of the car
	//It's better to guess too large than too small,
	//as large values will result in better stability
	float xs=2.5, ys=1.5, zs=5.0;
	m_InvMomentInertia.setElement(0,0,12*m_InvMass/(ys*ys+zs*zs));
	m_InvMomentInertia.setElement(1,1,12*m_InvMass/(xs*xs+zs*zs));
	m_InvMomentInertia.setElement(2,2,12*m_InvMass/(xs*xs+ys*ys));
	m_WheelRadius = 0.35; //meter

	m_WheelVelocity =  m_WheelAngle = 0.0;
	m_wheelHeight1 = m_wheelHeight2 = m_wheelHeight3 = m_wheelHeight4 = 0.0;

	m_Bodies.push_back(body);
	m_Bodies.push_back(wheel1);
	m_Bodies.push_back(wheel2);
	m_Bodies.push_back(wheel3);
	m_Bodies.push_back(wheel4);
}

CCar::~CCar(){
}

#define gasmax 25000
#define remmax 10000

void CCar::update(CPhysics *simulator, float dt)
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

	CMatrix &leftm = m_Bodies[1].m_OrientationMatrix;
	CMatrix &rightm = m_Bodies[2].m_OrientationMatrix;

	leftm.rotY(steeringangle);
	rightm.rotY(3.1416 + steeringangle);

	leftm = wheelmatrix * leftm;
	rightm = wheelmatrix * rightm;

	CMatrix flipRight;
	flipRight.rotY(3.1416); //TODO: faster CMatrix method

	m_Bodies[1].m_Position = CVector(-xfr, yfr - m_wheelHeight1, zfr);	//Left front
	m_Bodies[2].m_Position = CVector(xfr, yfr - m_wheelHeight2, zfr);	//Right front
	m_Bodies[3].m_Position = CVector(-xba, yba - m_wheelHeight3, zba);	//Left back
	m_Bodies[3].m_OrientationMatrix = wheelmatrix;
	m_Bodies[4].m_Position = CVector(xba, yba - m_wheelHeight4, zba);	//Right back
	m_Bodies[4].m_OrientationMatrix = wheelmatrix * flipRight;

	CMovingObject::update(simulator, dt);
}

void CCar::getForces(CVector &Ftot, CVector &Mtot)
{
	CMovingObject::getForces(Ftot, Mtot);

	CCarInput *input = (CCarInput *)m_InputData;

	//Gas
	Ftot += m_OrientationMatrix * CVector(0.0, 0.0, -gasmax * input->m_Forward);

	//Brake
	CVector direction = getMomentum();
	direction.normalise();
	Ftot -= direction * (remmax * input->m_Backward);

	//Steering
	Mtot += CVector(0,10000.0*input->m_Right,0);
}
