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

#include <cstdio>

#include "car.h"
#include "carinput.h"
#include "world.h"

#define remmax 100.0
#define steermax 0.5
#define maxtorque 5000.0

CCar::CCar()
{
	m_InputData = new CCarInput;

	m_Gear = 1;
	m_MainAxisVelocity = 0.0;
	m_MainAxisTorque = 0.0;
	m_gas = 0.0;

	//Five bodies:
	CBody body, wheel1, wheel2, wheel3, wheel4;
	body.m_Body = 0;
	wheel1.m_Body = 0;
	wheel2.m_Body = 0;
	wheel3.m_Body = 0;
	wheel4.m_Body = 0;

	body.createODE();
	wheel1.createODE();
	wheel2.createODE();
	wheel3.createODE();
	wheel4.createODE();


	//Just guessing the size of the car
	float xs=2.0, ys=1.2, zs=4.0;
	m_WheelRadius = 0.35; //35 cm

	dMass mbody, mwheel;
	
	dMassSetBox(&mbody, 1, xs, ys, zs);
	dMassAdjust(&mbody, 1000.0);
	dBodySetMass(body.m_ODEBody, &mbody);

	dMassSetSphere(&mwheel, 1, m_WheelRadius);
	dMassAdjust(&mwheel, 10.0);
	dBodySetMass(wheel1.m_ODEBody, &mwheel);
	dBodySetMass(wheel2.m_ODEBody, &mwheel);
	dBodySetMass(wheel3.m_ODEBody, &mwheel);
	dBodySetMass(wheel4.m_ODEBody, &mwheel);

	dBodySetFiniteRotationMode(wheel1.m_ODEBody, 1);
	dBodySetFiniteRotationMode(wheel2.m_ODEBody, 1);
	dBodySetFiniteRotationMode(wheel3.m_ODEBody, 1);
	dBodySetFiniteRotationMode(wheel4.m_ODEBody, 1);

	m_Bodies.push_back(body);
	m_Bodies.push_back(wheel1);
	m_Bodies.push_back(wheel2);
	m_Bodies.push_back(wheel3);
	m_Bodies.push_back(wheel4);

	//Setting the initial positions
	m_FrontWheelNeutral = CVector(0.8, 0.02, -1.75);
	m_BackWheelNeutral = CVector(0.8, 0.02, 1.1);

	resetBodyPositions(CVector(0,0,0), CMatrix());

	//Setting the connections
	m_joint1 = dJointCreateHinge2(theWorld->m_ODEWorld, 0);
	m_joint2 = dJointCreateHinge2(theWorld->m_ODEWorld, 0);
	m_joint3 = dJointCreateHinge2(theWorld->m_ODEWorld, 0);
	m_joint4 = dJointCreateHinge2(theWorld->m_ODEWorld, 0);
	dJointAttach(m_joint1, body.m_ODEBody, wheel1.m_ODEBody);
	dJointAttach(m_joint2, body.m_ODEBody, wheel2.m_ODEBody);
	dJointAttach(m_joint3, body.m_ODEBody, wheel3.m_ODEBody);
	dJointAttach(m_joint4, body.m_ODEBody, wheel4.m_ODEBody);
	CVector p1 = wheel1.getPosition();
	CVector p2 = wheel2.getPosition();
	CVector p3 = wheel3.getPosition();
	CVector p4 = wheel4.getPosition();
	dJointSetHinge2Anchor(m_joint1, p1.x, p1.y, p1.z);
	dJointSetHinge2Anchor(m_joint2, p2.x, p2.y, p2.z);
	dJointSetHinge2Anchor(m_joint3, p3.x, p3.y, p3.z);
	dJointSetHinge2Anchor(m_joint4, p4.x, p4.y, p4.z);
	dJointSetHinge2Axis1(m_joint1, 0,1,0);
	dJointSetHinge2Axis1(m_joint2, 0,1,0);
	dJointSetHinge2Axis1(m_joint3, 0,1,0);
	dJointSetHinge2Axis1(m_joint4, 0,1,0);
	dJointSetHinge2Axis2(m_joint1, 1,0,0);
	dJointSetHinge2Axis2(m_joint2, 1,0,0);
	dJointSetHinge2Axis2(m_joint3, 1,0,0);
	dJointSetHinge2Axis2(m_joint4, 1,0,0);

	//The stops
	dJointSetHinge2Param(m_joint1, dParamLoStop, -steermax);
	dJointSetHinge2Param(m_joint2, dParamLoStop, -steermax);
	dJointSetHinge2Param(m_joint3, dParamLoStop, 0);
	dJointSetHinge2Param(m_joint4, dParamLoStop, 0);
	dJointSetHinge2Param(m_joint1, dParamHiStop, steermax);
	dJointSetHinge2Param(m_joint2, dParamHiStop, steermax);
	dJointSetHinge2Param(m_joint3, dParamHiStop, 0);
	dJointSetHinge2Param(m_joint4, dParamHiStop, 0);
	dJointSetHinge2Param(m_joint1, dParamStopERP, 0.9);
	dJointSetHinge2Param(m_joint2, dParamStopERP, 0.9);
	dJointSetHinge2Param(m_joint3, dParamStopERP, 0.9);
	dJointSetHinge2Param(m_joint4, dParamStopERP, 0.9);
	dJointSetHinge2Param(m_joint1, dParamStopCFM, 0.001);
	dJointSetHinge2Param(m_joint2, dParamStopCFM, 0.001);
	dJointSetHinge2Param(m_joint3, dParamStopCFM, 0.001);
	dJointSetHinge2Param(m_joint4, dParamStopCFM, 0.001);
	dJointSetHinge2Param(m_joint1, dParamFudgeFactor, 0);
	dJointSetHinge2Param(m_joint2, dParamFudgeFactor, 0);
	dJointSetHinge2Param(m_joint3, dParamFudgeFactor, 0);
	dJointSetHinge2Param(m_joint4, dParamFudgeFactor, 0);
	dJointSetHinge2Param(m_joint1, dParamFMax, 10000.0);
	dJointSetHinge2Param(m_joint2, dParamFMax, 10000.0);
	dJointSetHinge2Param(m_joint3, dParamFMax, 10000.0);
	dJointSetHinge2Param(m_joint4, dParamFMax, 10000.0);

	//The suspension
	dJointSetHinge2Param(m_joint1, dParamSuspensionERP, 0.1);
	dJointSetHinge2Param(m_joint2, dParamSuspensionERP, 0.1);
	dJointSetHinge2Param(m_joint3, dParamSuspensionERP, 0.1);
	dJointSetHinge2Param(m_joint4, dParamSuspensionERP, 0.1);
	dJointSetHinge2Param(m_joint1, dParamSuspensionCFM, 0.00015);
	dJointSetHinge2Param(m_joint2, dParamSuspensionCFM, 0.00015);
	dJointSetHinge2Param(m_joint3, dParamSuspensionCFM, 0.00015);
	dJointSetHinge2Param(m_joint4, dParamSuspensionCFM, 0.00015);
	
	//One sound:
	m_Sounds.push_back(0);
}

CCar::~CCar()
{
}

void CCar::resetBodyPositions(CVector pos, const CMatrix &ori)
{
#define xfr	(m_FrontWheelNeutral.x)
#define xba	(m_BackWheelNeutral.x)
#define yfr	(m_FrontWheelNeutral.y)
#define yba	(m_BackWheelNeutral.y)
#define zfr	(m_FrontWheelNeutral.z)
#define zba	(m_BackWheelNeutral.z)

	CMatrix flipRight;
	flipRight.rotY(3.1416); //TODO: faster CMatrix method

	m_Bodies[0].setPosition(pos);	//Main body
	m_Bodies[1].setPosition(pos + CVector(-xfr, yfr, zfr));	//Left front
	m_Bodies[2].setPosition(pos + CVector(xfr, yfr, zfr));	//Right front
	m_Bodies[3].setPosition(pos + CVector(-xba, yba, zba));	//Left back
	m_Bodies[4].setPosition(pos + CVector(xba, yba, zba));	//Right back
	m_Bodies[0].setOrientationMatrix(ori);
	m_Bodies[1].setOrientationMatrix(ori);
	m_Bodies[2].setOrientationMatrix(ori * flipRight);
	m_Bodies[3].setOrientationMatrix(ori);
	m_Bodies[4].setOrientationMatrix(ori * flipRight);
}

#define cwA 0.01

void CCar::update(CPhysics *simulator, float dt)
{
	doSteering();

	m_Gear = ((CCarInput *)m_InputData)->m_Gear;
	updateAxisData();
	updateMainAxisVelocity();
	updateMainAxisTorque();
	applyWheelTorques();

	//----------------------
	//Air resistance
	//----------------------
	const dReal *vptr = dBodyGetLinearVel(m_Bodies[0].m_ODEBody);
	CVector v(vptr[0], vptr[1], vptr[2]);
	CVector F = -cwA * v.abs() * v;
	dBodyAddForce(m_Bodies[0].m_ODEBody, F.x, F.y, F.z);
	
	//----------------------
	//Common other things
	//----------------------
	CMovingObject::update(simulator, dt);
}

void CCar::updateAxisData()
{
	//This is what they *are*:
	CVector a1, a2, a3, a4;
	a1 = a2 = a3 = a4 = CVector(1,0,0);
	CMatrix ori1 = m_Bodies[1].getOrientationMatrix();
	CMatrix ori2 = m_Bodies[2].getOrientationMatrix();
	CMatrix ori3 = m_Bodies[3].getOrientationMatrix();
	CMatrix ori4 = m_Bodies[4].getOrientationMatrix();
	m_a1 = ori1 * a1;
	m_a2 = ori2 * a2;
	m_a3 = ori3 * a3;
	m_a4 = ori4 * a4;

	//This is what they *should* be (except for steering):
	a1 = a3 = CVector(1,0,0);
	a2 = a4 = CVector(-1,0,0);
	CMatrix ori0 = m_Bodies[0].getOrientationMatrix();
	a1 *= ori0;
	a2 *= ori0;
	a3 *= ori0;
	a4 *= ori0;

	//fprintf(stderr, "wheel 1: %.3f\n", a1.dotProduct(m_a1));
	//fprintf(stderr, "wheel 2: %.3f\n", a2.dotProduct(m_a2));
	//fprintf(stderr, "wheel 3: %.3f\n", a3.dotProduct(m_a3));
	//fprintf(stderr, "wheel 4: %.3f\n", a4.dotProduct(m_a4));

	//Set to the mean of the two:
	m_a1 = 0.5 * (a1 + m_a1);
	m_a2 = 0.5 * (a2 + m_a2);
	m_a3 = 0.5 * (a3 + m_a3);
	m_a4 = 0.5 * (a4 + m_a4);

	/*
	dBodySetFiniteRotationAxis(m_Bodies[1].m_ODEBody, a1.x, a1.y, a1.z);
	dBodySetFiniteRotationAxis(m_Bodies[2].m_ODEBody, a2.x, a2.y, a2.z);
	dBodySetFiniteRotationAxis(m_Bodies[3].m_ODEBody, a3.x, a3.y, a3.z);
	dBodySetFiniteRotationAxis(m_Bodies[4].m_ODEBody, a4.x, a4.y, a4.z);
	*/
}

void CCar::updateMainAxisVelocity() //from the wheel velocities
{
	dReal w1 = dJointGetHinge2Angle2Rate(m_joint1);
	dReal w2 = dJointGetHinge2Angle2Rate(m_joint2);
	//dReal w3 = dJointGetHinge2Angle2Rate(m_joint3);
	//dReal w4 = dJointGetHinge2Angle2Rate(m_joint4);

	m_MainAxisVelocity = 0.9 * m_MainAxisVelocity + 0.1 * 0.5 * (w1 + w2);
}

#define enginetorque 500.0
#define wcritical 500.0
#define dMdw (2*enginetorque / wcritical)
void CCar::updateMainAxisTorque()   //engine + gearbox simulation
{
	float wEngine = m_MainAxisVelocity / getGearRatio();

	/*
	|
	|------------\
	|             \
	|              \
	|               \
	|                \
	|                 \
	L__________________\_______
	|                   \
	|                    \_____
	*/

	float MEngine = enginetorque; //first part of the curve
	if(wEngine > wcritical)
	{
		MEngine = enginetorque - dMdw * (wEngine - wcritical);

		if(MEngine < -0.1 * enginetorque) MEngine = -0.1*enginetorque;
	}

	m_gas = ((CCarInput *)m_InputData)->m_Forward;
	MEngine *= m_gas;

	m_MainAxisTorque = MEngine / getGearRatio();
}

void CCar::applyWheelTorques()      //engine + brakes
{
	//angular velocities
	dReal w1 = dJointGetHinge2Angle2Rate(m_joint1);
	dReal w2 = dJointGetHinge2Angle2Rate(m_joint2);
	dReal w3 = dJointGetHinge2Angle2Rate(m_joint3);
	dReal w4 = dJointGetHinge2Angle2Rate(m_joint4);

	CCarInput *input = (CCarInput *)m_InputData;
	dReal rem = remmax * (0.01 + input->m_Backward);

	dReal m1 = 0.5 * m_MainAxisTorque - rem * w1;
	dReal m2 = 0.5 * m_MainAxisTorque - rem * w2;
	dReal m3 = -rem * w3;
	dReal m4 = -rem * w4;
	if(m1 > maxtorque) m1 = maxtorque;
	if(m1 < -maxtorque) m1 = -maxtorque;
	if(m2 > maxtorque) m2 = maxtorque;
	if(m2 < -maxtorque) m2 = -maxtorque;
	if(m3 > maxtorque) m3 = maxtorque;
	if(m3 < -maxtorque) m3 = -maxtorque;
	if(m4 > maxtorque) m4 = maxtorque;
	if(m4 < -maxtorque) m4 = -maxtorque;

	CVector M1 = -m1 * m_a1;
	CVector M2 = m2 * m_a2;
	CVector M3 = -m3 * m_a3;
	CVector M4 = m4 * m_a4;

	CVector Mt = -M1 -M2 -M3 -M4;
	dBodyAddTorque(m_Bodies[0].m_ODEBody, Mt.x, Mt.y, Mt.z);
	dBodyAddTorque(m_Bodies[1].m_ODEBody, M1.x, M1.y, M1.z);
	dBodyAddTorque(m_Bodies[2].m_ODEBody, M2.x, M2.y, M2.z);
	dBodyAddTorque(m_Bodies[3].m_ODEBody, M3.x, M3.y, M3.z);
	dBodyAddTorque(m_Bodies[4].m_ODEBody, M4.x, M4.y, M4.z);

	//dJointAddHinge2Torques(m_joint1, 0, Maxis);
	//dJointAddHinge2Torques(m_joint2, 0, Maxis);
	//dJointAddHinge2Torques(m_joint3, 0, Maxis);
	//dJointAddHinge2Torques(m_joint4, 0, Maxis);
}

float CCar::getGearRatio()
{
	switch(m_Gear)
	{
	case 0:
		return -0.1;
	case 1:
		return 0.1;
	case 2:
		return 0.2;
	case 3:
		return 0.4;
	case 4:
		return 0.8;
	case 5:
		return 1.1;
	case 6:
		return 1.5;
	}

	return 1.0;
}

void CCar::doSteering()
{
	CCarInput *input = (CCarInput *)m_InputData;
	dReal steer = steermax * input->m_Right;
	float angle1 = dJointGetHinge2Angle1(m_joint1);
	float angle2 = dJointGetHinge2Angle1(m_joint2);
	float v1 = steer - angle1;
	float v2 = steer - angle2;
	if(v1 > steermax) v1 = steermax;
	if(v1 < -steermax) v1 = -steermax;
	if(v2 > steermax) v2 = steermax;
	if(v2 < -steermax) v2 = -steermax;

	float min1 = steer, max1 = steer;
	if(angle1 < min1) min1 = angle1;
	if(angle1 > max1) max1 = angle1;
	float min2 = steer, max2 = steer;
	if(angle2 < min2) min2 = angle2;
	if(angle2 > max2) max2 = angle2;
	if(min1 < -steermax) min1 = -steermax;
	if(min2 < -steermax) min2 = -steermax;
	if(max1 > steermax)  max1 = steermax;
	if(max2 > steermax)  max2 = steermax;

	dJointSetHinge2Param(m_joint1, dParamVel, v1);
	dJointSetHinge2Param(m_joint2, dParamVel, v2);
	dJointSetHinge2Param(m_joint1, dParamLoStop, min1-0.0001);
	dJointSetHinge2Param(m_joint2, dParamLoStop, min2-0.0001);
	dJointSetHinge2Param(m_joint1, dParamHiStop, max1+0.0001);
	dJointSetHinge2Param(m_joint2, dParamHiStop, max2+0.0001);
}
