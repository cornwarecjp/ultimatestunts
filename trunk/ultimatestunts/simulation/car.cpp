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
#include <cmath>

#include "bound.h"
#include "car.h"
#include "carinput.h"
#include "world.h"

#include "datafile.h"
#include "lconfig.h"

CCar::CCar(CDataManager *manager) : CMovingObject(manager)
{
}

CCar::~CCar()
{
}

bool CCar::load(const CString &filename, const CParamList &list)
{
	CMovingObject::load(filename, list);

	CDataFile dfile(getFilename());
	CLConfig cfile(dfile.useExtern());
	//TODO: make a way to find out if this file exists
	//and return false if it doesn't

	//body
	CString bodygeomfile = cfile.getValue("body", "geometry");
	m_BodyMu = cfile.getValue("body", "mu").toFloat();
	m_BodyMass = cfile.getValue("body", "mass").toFloat();
	m_BodySize = cfile.getValue("body", "size").toVector();
	m_CameraPos = cfile.getValue("body", "camerapos").toVector();
	m_cwA = cfile.getValue("body", "cwa").toFloat();
	m_RotationDamping = cfile.getValue("body", "rotationdamping").toFloat();

	//wheels
	CString frontwheelgeomfile = cfile.getValue("frontwheels", "geometry");
	CString rearwheelgeomfile = cfile.getValue("rearwheels", "geometry");
	m_FrontWheelMu = cfile.getValue("frontwheels", "mu").toFloat();
	m_RearWheelMu = cfile.getValue("rearwheels", "mu").toFloat();
	m_FrontWheelMass = cfile.getValue("frontwheels", "mass").toFloat();
	m_RearWheelMass = cfile.getValue("rearwheels", "mass").toFloat();
	m_FrontWheelNeutral = cfile.getValue("frontwheels", "position").toVector();
	m_RearWheelNeutral = cfile.getValue("rearwheels", "position").toVector();
	m_FrontStopERP = cfile.getValue("frontwheels", "stoperp").toFloat();
	m_RearStopERP = cfile.getValue("rearwheels", "stoperp").toFloat();
	m_FrontStopCFM = cfile.getValue("frontwheels", "stopcfm").toFloat();
	m_RearStopCFM = cfile.getValue("rearwheels", "stopcfm").toFloat();
	m_FrontSuspERP = cfile.getValue("frontwheels", "susperp").toFloat();
	m_RearSuspERP = cfile.getValue("rearwheels", "susperp").toFloat();
	m_FrontSuspCFM = cfile.getValue("frontwheels", "suspcfm").toFloat();
	m_RearSuspCFM = cfile.getValue("rearwheels", "suspcfm").toFloat();
	m_FrontBrakeMax = cfile.getValue("frontwheels", "brakemax").toFloat();
	m_RearBrakeMax = cfile.getValue("rearwheels", "brakemax").toFloat();
	m_FrontSteerMax = cfile.getValue("frontwheels", "steermax").toFloat();
	m_RearSteerMax = -cfile.getValue("rearwheels", "steermax").toFloat();
	m_FrontTraction = cfile.getValue("frontwheels", "traction").toFloat();
	m_RearTraction = cfile.getValue("rearwheels", "traction").toFloat();

	//driving pipeline
	m_EngineTorque = cfile.getValue("engine", "enginetorque").toFloat();
	m_MaxEngineSpeed = cfile.getValue("engine", "maxenginespeed").toFloat();
	m_dMdw = 5*m_EngineTorque / m_MaxEngineSpeed;
	m_GearRatios.push_back(cfile.getValue("engine", "gear0").toFloat());
	m_GearRatios.push_back(cfile.getValue("engine", "gear1").toFloat());
	m_GearRatios.push_back(cfile.getValue("engine", "gear2").toFloat());
	m_GearRatios.push_back(cfile.getValue("engine", "gear3").toFloat());
	m_GearRatios.push_back(cfile.getValue("engine", "gear4").toFloat());
	m_GearRatios.push_back(cfile.getValue("engine", "gear5").toFloat());
	m_GearRatios.push_back(cfile.getValue("engine", "gear6").toFloat());
	m_DifferentialRatio = cfile.getValue("engine", "differentialratio").toFloat();

	//Two sounds:
	m_Sounds.push_back(theWorld->loadObject(cfile.getValue("sound", "engine"), CParamList(), CDataObject::eSample));
	m_Sounds.push_back(theWorld->loadObject(cfile.getValue("sound", "skid"), CParamList(), CDataObject::eSample));

	//One texture:
	m_Textures.push_back(theWorld->loadObject(cfile.getValue("texture", "file"), CParamList(), CDataObject::eMaterial));

	//The input object
	m_InputData = new CCarInput;

	//initial state
	m_Gear = 1;
	m_MainAxisVelocity = 0.0;
	m_MainAxisTorque = 0.0;
	m_gas = 0.0;

	//Five bodies:
	CBody body, wheel1, wheel2, wheel3, wheel4;

	//texture settings:
	CParamList plist;
	{
		SParameter p;
		p.name = "subset";
		p.value = (int)m_Textures[0];
		plist.push_back(p);
	}

	//Set the indices to the body array
	body.m_Body = theWorld->loadObject(bodygeomfile, plist, CDataObject::eBound);
	if(body.m_Body < 0)
		printf("Error: body geometry %s was not loaded\n", bodygeomfile.c_str());

	//The wheels are cylinders:
	{
		SParameter p;
		p.name = "cylinder";
		p.value = "true";
		plist.push_back(p);
	}
	
	wheel1.m_Body = theWorld->loadObject(frontwheelgeomfile, plist, CDataObject::eBound);
	wheel2.m_Body = wheel1.m_Body;
	if(wheel1.m_Body < 0)
		printf("Error: frontwheel geometry %s was not loaded\n", frontwheelgeomfile.c_str());
	m_FrontWheelRadius = ((CBound *)theWorld->getObject(CDataObject::eBound, wheel1.m_Body))->m_CylinderRadius;
	m_FrontWheelWidth = ((CBound *)theWorld->getObject(CDataObject::eBound, wheel1.m_Body))->m_CylinderWidth;

	wheel3.m_Body = theWorld->loadObject(rearwheelgeomfile, plist, CDataObject::eBound);
	wheel4.m_Body = wheel3.m_Body;
	if(wheel3.m_Body < 0)
		printf("Error: rearwheel geometry %s was not loaded\n", rearwheelgeomfile.c_str());
	m_RearWheelRadius = ((CBound *)theWorld->getObject(CDataObject::eBound, wheel3.m_Body))->m_CylinderRadius;
	m_RearWheelWidth = ((CBound *)theWorld->getObject(CDataObject::eBound, wheel3.m_Body))->m_CylinderWidth;

	body.m_mu = m_BodyMu;
	wheel1.m_mu = m_FrontWheelMu;
	wheel2.m_mu = m_FrontWheelMu;
	wheel3.m_mu = m_RearWheelMu;
	wheel4.m_mu = m_RearWheelMu;

	body.createODE();
	wheel1.createODE();
	wheel2.createODE();
	wheel3.createODE();
	wheel4.createODE();


	dMass mbody, mfrontwheel, mrearwheel;

	dMassSetBox(&mbody, 1, m_BodySize.x, m_BodySize.y, m_BodySize.z);
	dMassAdjust(&mbody, m_BodyMass);
	dBodySetMass(body.m_ODEBody, &mbody);

	dMassSetSphere(&mfrontwheel, 1, m_FrontWheelRadius);
	dMassAdjust(&mfrontwheel, m_FrontWheelMass);
	dBodySetMass(wheel1.m_ODEBody, &mfrontwheel);
	dBodySetMass(wheel2.m_ODEBody, &mfrontwheel);

	dMassSetSphere(&mrearwheel, 1, m_RearWheelRadius);
	dMassAdjust(&mrearwheel, m_RearWheelMass);
	dBodySetMass(wheel3.m_ODEBody, &mrearwheel);
	dBodySetMass(wheel4.m_ODEBody, &mrearwheel);

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

	m_DesiredSteering = m_MainAxisVelocity = 0.0;

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
	dJointSetHinge2Anchor(m_joint1, p1.x, p1.y, p1.z); // + 0.5 * m_FrontWheelWidth, p1.y, p1.z);
	dJointSetHinge2Anchor(m_joint2, p2.x, p2.y, p2.z); // - 0.5 * m_FrontWheelWidth, p2.y, p2.z);
	dJointSetHinge2Anchor(m_joint3, p3.x, p3.y, p3.z); // + 0.5 * m_RearWheelWidth, p3.y, p3.z);
	dJointSetHinge2Anchor(m_joint4, p4.x, p4.y, p4.z); // - 0.5 * m_RearWheelWidth, p4.y, p4.z);
	dJointSetHinge2Axis1(m_joint1, 0,1,0);
	dJointSetHinge2Axis1(m_joint2, 0,1,0);
	dJointSetHinge2Axis1(m_joint3, 0,1,0);
	dJointSetHinge2Axis1(m_joint4, 0,1,0);
	dJointSetHinge2Axis2(m_joint1, 1,0,0);
	dJointSetHinge2Axis2(m_joint2, 1,0,0);
	dJointSetHinge2Axis2(m_joint3, 1,0,0);
	dJointSetHinge2Axis2(m_joint4, 1,0,0);

	//The stops
	dJointSetHinge2Param(m_joint1, dParamLoStop, -m_FrontSteerMax);
	dJointSetHinge2Param(m_joint2, dParamLoStop, -m_FrontSteerMax);
	dJointSetHinge2Param(m_joint3, dParamLoStop, -m_RearSteerMax);
	dJointSetHinge2Param(m_joint4, dParamLoStop, -m_RearSteerMax);
	dJointSetHinge2Param(m_joint1, dParamHiStop, m_FrontSteerMax);
	dJointSetHinge2Param(m_joint2, dParamHiStop, m_FrontSteerMax);
	dJointSetHinge2Param(m_joint3, dParamHiStop, m_RearSteerMax);
	dJointSetHinge2Param(m_joint4, dParamHiStop, m_RearSteerMax);
	dJointSetHinge2Param(m_joint1, dParamStopERP, m_FrontStopERP);
	dJointSetHinge2Param(m_joint2, dParamStopERP, m_FrontStopERP);
	dJointSetHinge2Param(m_joint3, dParamStopERP, m_RearStopERP);
	dJointSetHinge2Param(m_joint4, dParamStopERP, m_RearStopERP);
	dJointSetHinge2Param(m_joint1, dParamStopCFM, m_FrontStopCFM);
	dJointSetHinge2Param(m_joint2, dParamStopCFM, m_FrontStopCFM);
	dJointSetHinge2Param(m_joint3, dParamStopCFM, m_RearStopCFM);
	dJointSetHinge2Param(m_joint4, dParamStopCFM, m_RearStopCFM);
	dJointSetHinge2Param(m_joint1, dParamFudgeFactor, 0);
	dJointSetHinge2Param(m_joint2, dParamFudgeFactor, 0);
	dJointSetHinge2Param(m_joint3, dParamFudgeFactor, 0);
	dJointSetHinge2Param(m_joint4, dParamFudgeFactor, 0);
	dJointSetHinge2Param(m_joint1, dParamFMax, 100000.0);
	dJointSetHinge2Param(m_joint2, dParamFMax, 100000.0);
	dJointSetHinge2Param(m_joint3, dParamFMax, 100000.0);
	dJointSetHinge2Param(m_joint4, dParamFMax, 100000.0);

	//The suspension
	dJointSetHinge2Param(m_joint1, dParamSuspensionERP, m_FrontSuspERP);
	dJointSetHinge2Param(m_joint2, dParamSuspensionERP, m_FrontSuspERP);
	dJointSetHinge2Param(m_joint3, dParamSuspensionERP, m_RearSuspERP);
	dJointSetHinge2Param(m_joint4, dParamSuspensionERP, m_RearSuspERP);
	dJointSetHinge2Param(m_joint1, dParamSuspensionCFM, m_FrontSuspCFM);
	dJointSetHinge2Param(m_joint2, dParamSuspensionCFM, m_FrontSuspCFM);
	dJointSetHinge2Param(m_joint3, dParamSuspensionCFM, m_RearSuspCFM);
	dJointSetHinge2Param(m_joint4, dParamSuspensionCFM, m_RearSuspCFM);

	return true;
}

void CCar::unload()
{
	//TODO
	CMovingObject::unload();
}

void CCar::resetBodyPositions(CVector pos, const CMatrix &ori)
{
#define xfr	(m_FrontWheelNeutral.x)
#define xba	(m_RearWheelNeutral.x)
#define yfr	(m_FrontWheelNeutral.y)
#define yba	(m_RearWheelNeutral.y)
#define zfr	(m_FrontWheelNeutral.z)
#define zba	(m_RearWheelNeutral.z)

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

void CCar::update(CPhysics *simulator, float dt)
{
	doSteering(dt);

	m_Gear = ((CCarInput *)m_InputData)->m_Gear;
	updateAxisData();
	updateMainAxisVelocity(dt);
	updateMainAxisTorque();
	applyWheelTorques();

	//----------------------
	//Air resistance
	//----------------------
	const dReal *vptr = dBodyGetLinearVel(m_Bodies[0].m_ODEBody);
	CVector v(vptr[0], vptr[1], vptr[2]);
	CVector F = -m_cwA * v.abs() * v;
	dBodyAddForce(m_Bodies[0].m_ODEBody, F.x, F.y, F.z);

	//----------------------
	//Rotation damping
	//----------------------
	const dReal *wptr = dBodyGetAngularVel(m_Bodies[0].m_ODEBody);
	CVector w(wptr[0], wptr[1], wptr[2]);
	CVector M = -m_RotationDamping * w.abs() * w;
	dBodyAddTorque(m_Bodies[0].m_ODEBody, M.x, M.y, M.z);
	
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

	//This is what they *should* be (including desired steering):
	a1 = a3 = CVector(1,0,0);
	a2 = a4 = CVector(-1,0,0);
	CMatrix ori0 = m_Bodies[0].getOrientationMatrix();
	a1 *= ori0;
	a2 *= ori0;
	a3 *= ori0;
	a4 *= ori0;

	CMatrix st1, st2, st3, st4;
	st1.rotY(m_DesiredSt1);
	st2.rotY(m_DesiredSt2);
	st3.rotY(m_DesiredSt3);
	st4.rotY(m_DesiredSt4);

	a1 *= st1;
	a2 *= st2;
	a3 *= st3;
	a4 *= st4;

	//fprintf(stderr, "wheel 1: %.3f\n", a1.dotProduct(m_a1));
	//fprintf(stderr, "wheel 2: %.3f\n", a2.dotProduct(m_a2));
	//fprintf(stderr, "wheel 3: %.3f\n", a3.dotProduct(m_a3));
	//fprintf(stderr, "wheel 4: %.3f\n", a4.dotProduct(m_a4));

	//Set to the mean of the two:
	/*
	m_a1 = 0.5 * (a1 + m_a1);
	m_a2 = 0.5 * (a2 + m_a2);
	m_a3 = 0.5 * (a3 + m_a3);
	m_a4 = 0.5 * (a4 + m_a4);
	*/
	m_a1 = a1;
	m_a2 = a2;
	m_a3 = a3;
	m_a4 = a4;

	/*
	dBodySetFiniteRotationAxis(m_Bodies[1].m_ODEBody, a1.x, a1.y, a1.z);
	dBodySetFiniteRotationAxis(m_Bodies[2].m_ODEBody, a2.x, a2.y, a2.z);
	dBodySetFiniteRotationAxis(m_Bodies[3].m_ODEBody, a3.x, a3.y, a3.z);
	dBodySetFiniteRotationAxis(m_Bodies[4].m_ODEBody, a4.x, a4.y, a4.z);
	*/
}

void CCar::updateMainAxisVelocity(float dt) //from the wheel velocities
{
	/*
	const dReal *angvel = dBodyGetAngularVel(m_Bodies[1].m_ODEBody);
	dReal w1 = sqrt(angvel[0]*angvel[0] + angvel[1]*angvel[1] + angvel[2]*angvel[2]);
	angvel = dBodyGetAngularVel(m_Bodies[2].m_ODEBody);
	dReal w2 = sqrt(angvel[0]*angvel[0] + angvel[1]*angvel[1] + angvel[2]*angvel[2]);
	*/
	
	dReal w1 = dJointGetHinge2Angle2Rate(m_joint1);
	dReal w2 = dJointGetHinge2Angle2Rate(m_joint2);
	dReal w3 = dJointGetHinge2Angle2Rate(m_joint3);
	dReal w4 = dJointGetHinge2Angle2Rate(m_joint4);

	float factor = exp(-100.0*dt);
	m_MainAxisVelocity = factor * m_MainAxisVelocity + (1.0-factor) * 0.25 * (w1 + w2 + w3 + w4);
}

float CCar::getEngineTorque(float wengine)
{
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

	float MEngine = m_EngineTorque; //first part of the curve
	if(wengine > m_MaxEngineSpeed)
	{
		MEngine = m_EngineTorque - m_dMdw * (wengine - m_MaxEngineSpeed);

		if(MEngine < -0.1 * m_EngineTorque) MEngine = -0.1*m_EngineTorque;
	}

	return MEngine;
}

void CCar::updateMainAxisTorque()   //engine + gearbox simulation
{
	float MEngine = getEngineTorque(m_MainAxisVelocity * getGearRatio());

	m_gas = ((CCarInput *)m_InputData)->m_Forward;
	MEngine *= m_gas;

	m_MainAxisTorque = MEngine * getGearRatio();
}

void CCar::applyWheelTorques()      //engine + brakes
{
	//angular velocities
	dReal w1 = dJointGetHinge2Angle2Rate(m_joint1);
	dReal w2 = dJointGetHinge2Angle2Rate(m_joint2);
	dReal w3 = dJointGetHinge2Angle2Rate(m_joint3);
	dReal w4 = dJointGetHinge2Angle2Rate(m_joint4);

	CCarInput *input = (CCarInput *)m_InputData;
	dReal frontbrake = m_FrontBrakeMax * input->m_Backward;
	dReal rearbrake = m_RearBrakeMax * input->m_Backward;

	dReal m1 = 0.5 * (m_FrontTraction * m_MainAxisTorque - frontbrake * w1);
	dReal m2 = 0.5 * (m_FrontTraction * m_MainAxisTorque - frontbrake * w2);
	dReal m3 = 0.5 * (m_RearTraction * m_MainAxisTorque - rearbrake * w3);
	dReal m4 = 0.5 * (m_RearTraction * m_MainAxisTorque - rearbrake * w4);

	/*
	if(m1 > m_MaxTorque) m1 = m_MaxTorque;
	if(m1 < -m_MaxTorque) m1 = -m_MaxTorque;
	if(m2 > m_MaxTorque) m2 = m_MaxTorque;
	if(m2 < -m_MaxTorque) m2 = -m_MaxTorque;
	if(m3 > m_MaxTorque) m3 = m_MaxTorque;
	if(m3 < -m_MaxTorque) m3 = -m_MaxTorque;
	if(m4 > m_MaxTorque) m4 = m_MaxTorque;
	if(m4 < -m_MaxTorque) m4 = -m_MaxTorque;
	*/

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

float CCar::getGearRatio(int gear)
{
	if(gear < 0) gear = m_Gear;
	return m_DifferentialRatio * m_GearRatios[gear];
}

void CCar::doSteering(float dt)
{
	CCarInput *input = (CCarInput *)m_InputData;
	dReal steer = input->m_Right;

	float factor = exp(-1.0*dt);
	if(fabsf(steer) < fabsf(m_DesiredSteering))
		factor = exp(-25.0*dt);

	m_DesiredSteering = factor * m_DesiredSteering + (1.0-factor) * steer;

	//current angles
	float angle1 = dJointGetHinge2Angle1(m_joint1);
	float angle2 = dJointGetHinge2Angle1(m_joint2);
	float angle3 = dJointGetHinge2Angle1(m_joint3);
	float angle4 = dJointGetHinge2Angle1(m_joint4);

	//if the wheels were in the middle of the car
	float desiredfront = m_FrontSteerMax * m_DesiredSteering;
	float desiredrear = m_RearSteerMax * m_DesiredSteering;

	//desired angles
	//important: default to zero
	m_DesiredSt1 = 0.0, m_DesiredSt2 = 0.0, m_DesiredSt3 = 0.0, m_DesiredSt4 = 0.0;

	if(fabs(desiredfront) > 0.0001 || fabs(desiredrear) > 0.0001)
	{
		//This correction is actually better than that
		//is possible in a real car
		float lth = m_RearWheelNeutral.z  - m_FrontWheelNeutral.z;
		float halfwthf = m_FrontWheelNeutral.x;
		float halfwthr = m_RearWheelNeutral.x;
		float tanaf = tanf(desiredfront);
		float tanar = tanf(-desiredrear);
		float yf = lth * tanaf / (tanar + tanaf);
		float yr = lth - yf;
		float x;
		if(fabs(tanaf) > 0.0001)
			{x = 0.5*lth / tanaf;} //yf / tanaf}
		else
			{x = yr / tanar;}

		m_DesiredSt1 = atanf(yf / (x + halfwthf));
		m_DesiredSt2 = atanf(yf / (x - halfwthf));
		m_DesiredSt3 = -atanf(yr / (x + halfwthr));
		m_DesiredSt4 = -atanf(yr / (x - halfwthr));
	}
	

	//desired angles
	//m_DesiredSt1 = desiredfront;
	//m_DesiredSt2 = desiredfront;
	//m_DesiredSt3 = desiredrear;
	//m_DesiredSt4 = desiredrear;

	float v1 = m_DesiredSt1 - angle1;
	float v2 = m_DesiredSt2 - angle2;
	float v3 = m_DesiredSt3 - angle3;
	float v4 = m_DesiredSt4 - angle4;

	dJointSetHinge2Param(m_joint1, dParamVel, 10.0*v1);
	dJointSetHinge2Param(m_joint2, dParamVel, 10.0*v2);
	dJointSetHinge2Param(m_joint3, dParamVel, 10.0*v3);
	dJointSetHinge2Param(m_joint4, dParamVel, 10.0*v4);
	dJointSetHinge2Param(m_joint1, dParamLoStop, m_DesiredSt1-0.00001);
	dJointSetHinge2Param(m_joint2, dParamLoStop, m_DesiredSt2-0.00001);
	dJointSetHinge2Param(m_joint3, dParamLoStop, m_DesiredSt3-0.00001);
	dJointSetHinge2Param(m_joint4, dParamLoStop, m_DesiredSt4-0.00001);
	dJointSetHinge2Param(m_joint1, dParamHiStop, m_DesiredSt1+0.00001);
	dJointSetHinge2Param(m_joint2, dParamHiStop, m_DesiredSt2+0.00001);
	dJointSetHinge2Param(m_joint3, dParamHiStop, m_DesiredSt3+0.00001);
	dJointSetHinge2Param(m_joint4, dParamHiStop, m_DesiredSt4+0.00001);
}
