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

CCar::CCar()
{
	m_InputData = new CCarInput;

	m_cwA = 5.0;

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
	float xs=2.0, ys=1.3, zs=4.0;
	m_WheelRadius = 0.35; //meter

	dMass mbody, mwheel;
	
	dMassSetBox(&mbody, 1, xs, ys, zs);
	dMassAdjust(&mbody, 1000.0);
	dBodySetMass(body.m_ODEBody, &mbody);

	dMassSetSphere(&mwheel, 1, m_WheelRadius);
	dMassAdjust(&mwheel, 1.0);
	dBodySetMass(wheel1.m_ODEBody, &mwheel);
	dBodySetMass(wheel2.m_ODEBody, &mwheel);
	dBodySetMass(wheel3.m_ODEBody, &mwheel);
	dBodySetMass(wheel4.m_ODEBody, &mwheel);

	dBodySetFiniteRotationMode(wheel1.m_ODEBody, 1);
	dBodySetFiniteRotationMode(wheel2.m_ODEBody, 1);
	dBodySetFiniteRotationMode(wheel3.m_ODEBody, 1);
	dBodySetFiniteRotationMode(wheel4.m_ODEBody, 1);
	dBodySetFiniteRotationAxis(wheel1.m_ODEBody, 0, 0, 0);
	dBodySetFiniteRotationAxis(wheel2.m_ODEBody, 0, 0, 0);
	dBodySetFiniteRotationAxis(wheel3.m_ODEBody, 0, 0, 0);
	dBodySetFiniteRotationAxis(wheel4.m_ODEBody, 0, 0, 0);

	m_Bodies.push_back(body);
	m_Bodies.push_back(wheel1);
	m_Bodies.push_back(wheel2);
	m_Bodies.push_back(wheel3);
	m_Bodies.push_back(wheel4);

	//Setting the initial positions
	m_FrontWheelNeutral = CVector(0.8, 0.1, -1.75);
	m_BackWheelNeutral = CVector(0.8, 0.1, 1.1);

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
	dJointSetHinge2Param(m_joint1, dParamLoStop, -0.00001);
	dJointSetHinge2Param(m_joint2, dParamLoStop, -0.00001);
	dJointSetHinge2Param(m_joint3, dParamLoStop, -0.00001);
	dJointSetHinge2Param(m_joint4, dParamLoStop, -0.00001);
	dJointSetHinge2Param(m_joint1, dParamHiStop, 0.00001);
	dJointSetHinge2Param(m_joint2, dParamHiStop, 0.00001);
	dJointSetHinge2Param(m_joint3, dParamHiStop, 0.00001);
	dJointSetHinge2Param(m_joint4, dParamHiStop, 0.00001);
	dJointSetHinge2Param(m_joint1, dParamSuspensionERP, 0.99);
	dJointSetHinge2Param(m_joint2, dParamSuspensionERP, 0.99);
	dJointSetHinge2Param(m_joint3, dParamSuspensionERP, 0.99);
	dJointSetHinge2Param(m_joint4, dParamSuspensionERP, 0.99);
	dJointSetHinge2Param(m_joint1, dParamSuspensionCFM, 0.0001);
	dJointSetHinge2Param(m_joint2, dParamSuspensionCFM, 0.0001);
	dJointSetHinge2Param(m_joint3, dParamSuspensionCFM, 0.0001);
	dJointSetHinge2Param(m_joint4, dParamSuspensionCFM, 0.0001);

	//The collision info
	dGeomID theBox = dCreateBox(0, xs, ys, zs);
	dGeomSetPosition(theBox, 0.0, 0.5, 0.0); //relative to transform geom
	
	m_body = dCreateGeomTransform(0);
	dGeomTransformSetGeom(m_body, theBox);
	dGeomTransformSetCleanup(m_body, 1);
	dGeomTransformSetInfo(m_body, 1);

	dGeomSetBody(m_body, body.m_ODEBody);

	m_wheel1 = dCreateSphere(0, m_WheelRadius);
	m_wheel2 = dCreateSphere(0, m_WheelRadius);
	m_wheel3 = dCreateSphere(0, m_WheelRadius);
	m_wheel4 = dCreateSphere(0, m_WheelRadius);
	dGeomSetBody(m_wheel1, wheel1.m_ODEBody);
	dGeomSetBody(m_wheel2, wheel2.m_ODEBody);
	dGeomSetBody(m_wheel3, wheel3.m_ODEBody);
	dGeomSetBody(m_wheel4, wheel4.m_ODEBody);

	m_geomgroup = dCreateGeomGroup(theWorld->m_Space);
	dGeomGroupAdd(m_geomgroup, m_body);
	dGeomGroupAdd(m_geomgroup, m_wheel1);
	dGeomGroupAdd(m_geomgroup, m_wheel2);
	dGeomGroupAdd(m_geomgroup, m_wheel3);
	dGeomGroupAdd(m_geomgroup, m_wheel4);

	
	//One sound:
	m_Sounds.push_back(0);
}

CCar::~CCar()
{
	dGeomDestroy(m_body);
	dGeomDestroy(m_wheel1);
	dGeomDestroy(m_wheel2);
	dGeomDestroy(m_wheel3);
	dGeomDestroy(m_wheel4);
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

#define gasmax 2000.0
#define remmax 10.0
#define steermax 0.5

void CCar::update(CPhysics *simulator, float dt)
{
	CCarInput *input = (CCarInput *)m_InputData;

	//----------------------
	//Steering
	//----------------------

	dReal steer = steermax * input->m_Right;
	dReal v1 = steer - dJointGetHinge2Angle1(m_joint1);
	dReal v2 = steer - dJointGetHinge2Angle1(m_joint2);
	if(v1 > steermax) v1 = steermax;
	if(v1 < -steermax) v1 = -steermax;
	if(v2 > steermax) v2 = steermax;
	if(v2 < -steermax) v2 = -steermax;

	dJointSetHinge2Param(m_joint1, dParamVel, 5*v1);
	dJointSetHinge2Param(m_joint2, dParamVel, 5*v2);
	dJointSetHinge2Param(m_joint1, dParamFMax, 100000.0);
	dJointSetHinge2Param(m_joint2, dParamFMax, 100000.0);
	dJointSetHinge2Param(m_joint1, dParamLoStop, -steermax);
	dJointSetHinge2Param(m_joint2, dParamLoStop, -steermax);
	dJointSetHinge2Param(m_joint1, dParamHiStop, steermax);
	dJointSetHinge2Param(m_joint2, dParamHiStop, steermax);

	/*
	dJointSetHinge2Param(m_joint1, dParamLoStop, steer - 0.0001);
	dJointSetHinge2Param(m_joint2, dParamLoStop, steer - 0.0001);
	dJointSetHinge2Param(m_joint1, dParamHiStop, steer + 0.0001);
	dJointSetHinge2Param(m_joint2, dParamHiStop, steer + 0.0001);
	*/
	
	//dJointSetHinge2Param(m_joint1, dParamFudgeFactor, 0.1);
	//dJointSetHinge2Param(m_joint2, dParamFudgeFactor, 0.1);
	dJointSetHinge2Param(m_joint1, dParamStopERP, 0.99);
	dJointSetHinge2Param(m_joint2, dParamStopERP, 0.99);
	dJointSetHinge2Param(m_joint1, dParamStopCFM, 0.0001);
	dJointSetHinge2Param(m_joint2, dParamStopCFM, 0.0001);

	//----------------------
	//Gas and brake
	//----------------------

	//axis directions
	CVector a1, a2, a3, a4;
	a1 = a2 = a3 = a4 = CVector(1,0,0);
	CMatrix ori1 = m_Bodies[1].getOrientationMatrix();
	CMatrix ori2 = m_Bodies[2].getOrientationMatrix();
	CMatrix ori3 = m_Bodies[3].getOrientationMatrix();
	CMatrix ori4 = m_Bodies[4].getOrientationMatrix();
	a1 *= ori1;
	a2 *= ori2;
	a3 *= ori3;
	a4 *= ori4;

	//angular velocities
	dReal w1 = dJointGetHinge2Angle2Rate(m_joint1);
	dReal w2 = dJointGetHinge2Angle2Rate(m_joint2);
	dReal w3 = dJointGetHinge2Angle2Rate(m_joint3);
	dReal w4 = dJointGetHinge2Angle2Rate(m_joint4);

	dReal Mgas = gasmax * input->m_Forward;
	dReal rem = remmax * (0.05 + input->m_Backward);

	dReal m1 = Mgas - rem * w1;
	dReal m2 = Mgas - rem * w2;
	dReal m3 = -rem * w3;
	dReal m4 = -rem * w4;

	CVector M1 = -m1 * a1;
	CVector M2 = m2 * a2;
	CVector M3 = -m3 * a3;
	CVector M4 = m4 * a4;

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

	//----------------------
	//Air resistance
	//----------------------
	const dReal *vptr = dBodyGetLinearVel(m_Bodies[0].m_ODEBody);
	CVector v(vptr[0], vptr[1], vptr[2]);
	CVector F = -m_cwA * v.abs() * v;
	dBodyAddForce(m_Bodies[0].m_ODEBody, F.x, F.y, F.z);
	
	//----------------------
	//Common other things
	//----------------------
	CMovingObject::update(simulator, dt);
}
