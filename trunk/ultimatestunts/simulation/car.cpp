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

#ifndef M_PI
#define M_PI 3.1415926536
#endif
#define DBLPI (2.0*M_PI)

#include "bound.h"
#include "car.h"
#include "carinput.h"
#include "world.h"

#include "physics.h"

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

	//Mass
	m_InvMass = 1.0 / cfile.getValue("body", "mass").toFloat();
	m_BodySize = cfile.getValue("body", "size").toVector();
	m_InvInertia *= m_InvMass;

	//body
	CString bodygeomfile = cfile.getValue("body", "geometry");
	m_BodyMu = cfile.getValue("body", "mu").toFloat();
	m_CameraPos = cfile.getValue("body", "camerapos").toVector();
	m_cwA = cfile.getValue("body", "cwa").toFloat();
	m_RotationDamping = cfile.getValue("body", "rotationdamping").toFloat();

	//wheels
	CString frontwheelgeomfile = cfile.getValue("frontwheels", "geometry");
	CString rearwheelgeomfile = cfile.getValue("rearwheels", "geometry");
	m_FrontWheelMu = cfile.getValue("frontwheels", "mu").toFloat();
	m_RearWheelMu = cfile.getValue("rearwheels", "mu").toFloat();
	m_FrontWheelNeutral = cfile.getValue("frontwheels", "position").toVector();
	m_RearWheelNeutral = cfile.getValue("rearwheels", "position").toVector();
	m_FrontBrakeMax = cfile.getValue("frontwheels", "brakemax").toFloat();
	m_RearBrakeMax = cfile.getValue("rearwheels", "brakemax").toFloat();
	m_FrontSteerMax = cfile.getValue("frontwheels", "steermax").toFloat();
	m_RearSteerMax = -cfile.getValue("rearwheels", "steermax").toFloat();
	m_FrontTraction = cfile.getValue("frontwheels", "traction").toFloat();
	m_RearTraction = cfile.getValue("rearwheels", "traction").toFloat();
	m_FrontDownforce = cfile.getValue("frontwheels", "downforce").toFloat();
	m_RearDownforce = cfile.getValue("rearwheels", "downforce").toFloat();

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

	//The input object: CCarInput instead of CMovObjInput
	delete m_InputData;
	m_InputData = new CCarInput;
	m_InputData->m_MovObjID = m_MovObjID;

	//initial state
	m_Gear = 1;
	m_MainAxisTorque = 0.0;
	m_gas = 0.0;
	m_a1 = m_a2 = m_a3 = m_a4 = 0.0;
	m_w1 = m_w2 = m_w3 = m_w4 = 0.0;
	m_M1 = m_M2 = m_M3 = m_M4 = 0.0;
	m_DesiredSteering = m_DesiredSt1 = m_DesiredSt2 = m_DesiredSt3 = m_DesiredSt4 = 0.0;
	m_MainAxisVelocity = 0.0;

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

	/*
	//Temporary. TODO: replace ODE things
	body.createODE();
	wheel1.createODE();
	wheel2.createODE();
	wheel3.createODE();
	wheel4.createODE();
	*/

	m_Bodies.push_back(body);
	m_Bodies.push_back(wheel1);
	m_Bodies.push_back(wheel2);
	m_Bodies.push_back(wheel3);
	m_Bodies.push_back(wheel4);

	//Setting the initial positions
	resetBodyPositions(CVector(0,0,0), CMatrix());

	return true;
}

void CCar::unload()
{
	//TODO
	CMovingObject::unload();
}

void CCar::resetBodyPositions(CVector pos, const CMatrix &ori)
{
	m_Position = pos;	//Main body
	m_OrientationMatrix = ori;

	placeBodies();
}

void CCar::placeBodies()
{
#define xfr	(m_FrontWheelNeutral.x)
#define xba	(m_RearWheelNeutral.x)
#define yfr	(m_FrontWheelNeutral.y)
#define yba	(m_RearWheelNeutral.y)
#define zfr	(m_FrontWheelNeutral.z)
#define zba	(m_RearWheelNeutral.z)

	//wheel steering matrices
	CMatrix s1, s2, s3, s4;
	s1.rotY(m_DesiredSt1);
	s2.rotY(M_PI+m_DesiredSt2);
	s3.rotY(m_DesiredSt3);
	s4.rotY(M_PI+m_DesiredSt4);

	//wheel rotation matrices
	CMatrix r1, r2, r3, r4;
	r1.rotX(m_a1);
	r2.rotX(-m_a2);
	r3.rotX(m_a3);
	r4.rotX(-m_a4);

	m_Bodies[0].m_Position = m_Position;
	m_Bodies[0].m_OrientationMatrix = m_OrientationMatrix;

	m_Bodies[1].m_Position = m_Position + m_OrientationMatrix * CVector(-xfr, yfr, zfr);	//Left front
	m_Bodies[2].m_Position = m_Position + m_OrientationMatrix * CVector(xfr, yfr, zfr);	//Right front
	m_Bodies[3].m_Position = m_Position + m_OrientationMatrix * CVector(-xba, yba, zba);	//Left back
	m_Bodies[4].m_Position = m_Position + m_OrientationMatrix * CVector(xba, yba, zba);	//Right back

	m_Bodies[1].m_OrientationMatrix = r1 * s1 * m_OrientationMatrix;
	m_Bodies[2].m_OrientationMatrix = r2 * s2 * m_OrientationMatrix;
	m_Bodies[3].m_OrientationMatrix = r3 * s3 * m_OrientationMatrix;
	m_Bodies[4].m_OrientationMatrix = r4 * s4 * m_OrientationMatrix;
}

void CCar::placeOnGround(const CCollisionFace *theGround)
{
	//fprintf(stderr, "ground normal = %s\n", CString(theGround->nor).c_str());

	//The right rotation:
	
	CMatrix &Mnu = m_OrientationMatrix;
	CVector ynu;
	ynu.x = Mnu.Element(1,0);
	ynu.y = Mnu.Element(1,1);
	ynu.z = Mnu.Element(1,2);

	CVector ystr = theGround->nor;
	CVector cp = ystr.crossProduct(ynu);
	
	float abscp = cp.abs();
	if(abscp > 0.001) cp *= (asin(abscp) / abscp);

	CMatrix rotate;
	rotate.setRotation(cp);

	Mnu *= rotate;

	//The right position:

	CVector &pos = m_Position;
	float dobj = pos.dotProduct(theGround->nor) - theGround->d;
	pos -= (dobj - 0.5) * theGround->nor;
	
	//align the linear velocity:
	float vvert = m_Velocity.dotProduct(theGround->nor);
	if(vvert < 0.0)
	{
		m_Velocity -= vvert * theGround->nor;
	}

	//align the angular velocity
	m_AngularVelocity = m_AngularVelocity.component(theGround->nor);

	//placeBodies(); //not needed
}

void CCar::update(CPhysics *simulator, float dt)
{
	doSteering(dt);

	const CCollisionFace *theGround = simulator->getGroundFace(m_Position);

	simulateGeneral(simulator, dt);
	if(theGround == NULL)
	{
		//fprintf(stderr, "ground plane NOT found\n");
		simulateAir(simulator, dt);
	}
	else
	{
		float vvert = m_Velocity.dotProduct(theGround->nor);
		float dobj = m_Bodies[0].m_Position.dotProduct(theGround->nor) - theGround->d;
		if(vvert > 0.1 || dobj > 2.0) //TODO: calculate this value
		{
			//fprintf(stderr, "ground plane far below\n");
			simulateAir(simulator, dt);
		}
		else
		{
			//fprintf(stderr, "ground plane found\n");
			placeOnGround(theGround);
			simulateGround(simulator, dt);
		}
	}


	//the wheels
	m_a1 += m_w1 * dt;
	m_a2 += m_w2 * dt;
	m_a3 += m_w3 * dt;
	m_a4 += m_w4 * dt;
	if(m_a1 > DBLPI) m_a1 -= DBLPI;
	if(m_a2 > DBLPI) m_a2 -= DBLPI;
	if(m_a3 > DBLPI) m_a3 -= DBLPI;
	if(m_a4 > DBLPI) m_a4 -= DBLPI;
	if(m_a1 < 0.0) m_a1 += DBLPI;
	if(m_a2 < 0.0) m_a2 += DBLPI;
	if(m_a3 < 0.0) m_a3 += DBLPI;
	if(m_a4 < 0.0) m_a4 += DBLPI;

	m_w1 += dt * m_M1 / 10.0; //TODO: fill in the wheel inertia
	m_w2 += dt * m_M2 / 10.0;
	m_w3 += dt * m_M3 / 10.0;
	m_w4 += dt * m_M4 / 10.0;

	//----------------------
	//Integration step and other general things
	//----------------------
	CMovingObject::update(simulator, dt);
}

void CCar::simulateGeneral(CPhysics *simulator, float dt)
{
	/*
	I know that this defies the law of gravity, but,
	you see, I never studied law.

	Bugs Bunny
	*/
	addForce(CVector(0,-9.81 / m_InvMass, 0));

	addDownforce();

	//----------------------
	//Air resistance
	//----------------------
	addForce(-m_cwA * m_Velocity.abs() * m_Velocity);

	//----------------------
	//Rotation damping
	//----------------------
	addTorque(-m_RotationDamping * m_AngularVelocity.abs() * m_AngularVelocity);

	//----------------------
	//Engine simulation
	//----------------------
	m_Gear = ((CCarInput *)m_InputData)->m_Gear;
	updateWheelOrientation();
	updateMainAxisVelocity(dt);
	updateMainAxisTorque();
	updateWheelTorques();
}

void CCar::simulateAir(CPhysics *simulator, float dt)
{
	//maybe there is something...
}

void CCar::simulateGround(CPhysics *simulator, float dt)
{
	applyWheelForces();
}

void CCar::addDownforce()
{
	const CMatrix &ori = m_Bodies[0].m_OrientationMatrix;
	
	CVector v = m_Velocity;

	v /= ori;

	if(v.z < 0.0) //moving forward
	{
		//TODO

		/*
		dBodyAddRelForceAtRelPos(m_Bodies[0].m_ODEBody,
			0.0, -m_FrontDownforce*v.z*v.z, 0.0,
			0.0, 0.0, m_FrontWheelNeutral.z);

		dBodyAddRelForceAtRelPos(m_Bodies[0].m_ODEBody,
			0.0, -m_RearDownforce*v.z*v.z, 0.0,
			0.0, 0.0, m_RearWheelNeutral.z);
		*/
	}
}

void CCar::doSteering(float dt)
{
	CCarInput *input = (CCarInput *)m_InputData;
	float steer = input->m_Right;

	float factor = exp(-1.0*dt);
	if(fabsf(steer) < fabsf(m_DesiredSteering))
		factor = exp(-25.0*dt);

	m_DesiredSteering = factor * m_DesiredSteering + (1.0-factor) * steer;

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

}

void CCar::updateWheelOrientation()
{
	//body orientation:
	CVector zaxis;
	zaxis.x = m_OrientationMatrix.Element(2,0);
	zaxis.y = m_OrientationMatrix.Element(2,1);
	zaxis.z = m_OrientationMatrix.Element(2,2);

	//steering matrices
	CMatrix s1, s2, s3, s4;
	s1.rotY(m_DesiredSt1);
	s2.rotY(m_DesiredSt2);
	s3.rotY(m_DesiredSt3);
	s4.rotY(m_DesiredSt4);

	m_Zwheel1 = s1 * zaxis;
	m_Zwheel2 = s2 * zaxis;
	m_Zwheel3 = s3 * zaxis;
	m_Zwheel4 = s4 * zaxis;
}

void CCar::updateMainAxisVelocity(float dt) //from the wheel velocities
{
	float factor = exp(-100.0*dt);
	m_MainAxisVelocity = factor * m_MainAxisVelocity + (1.0-factor) * 0.25 * (m_w1 + m_w2 + m_w3 + m_w4);
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

float CCar::getGearRatio(int gear)
{
	if(gear < 0) gear = m_Gear;
	return m_DifferentialRatio * m_GearRatios[gear];
}

void CCar::updateWheelTorques()      //engine + brakes
{
	CCarInput *input = (CCarInput *)m_InputData;
	float frontbrake = m_FrontBrakeMax * input->m_Backward;
	float rearbrake = m_RearBrakeMax * input->m_Backward;

	m_M1 = 0.5 * (m_FrontTraction * m_MainAxisTorque - frontbrake * m_w1);
	m_M2 = 0.5 * (m_FrontTraction * m_MainAxisTorque - frontbrake * m_w2);
	m_M3 = 0.5 * (m_RearTraction * m_MainAxisTorque - rearbrake * m_w3);
	m_M4 = 0.5 * (m_RearTraction * m_MainAxisTorque - rearbrake * m_w4);
}

void CCar::applyWheelForces()
{
	//The contact parameters:
	float staticfric = 1.3;
	float mrotf = 0.01;
	float cornerstiffness = 0.5;
	float tractionconstant = 2.0;

	//This function mainly works in the car coordinate system
	CMatrix Rmat_inv = m_OrientationMatrix.transpose();

	//Wheel z axes:
	CVector wz[4];
	wz[0] = Rmat_inv * m_Zwheel1;
	wz[1] = Rmat_inv * m_Zwheel2;
	wz[2] = Rmat_inv * m_Zwheel3;
	wz[3] = Rmat_inv * m_Zwheel4;

	//required normal force:
	CVector Fext = Rmat_inv * m_Ftot;
	float Fy_tot = -Fext.y;
	if(Fy_tot < 0.0) Fy_tot = 0.0; //no "glue" forces

	//position of the wheels:
	CVector r[4];
	r[0] = CVector(-xfr, yfr, zfr);	//Left front
	r[1] = CVector(xfr, yfr, zfr);	//Right front
	r[2] = CVector(-xba, yba, zba);	//Left back
	r[3] = CVector(xba, yba, zba);	//Right back

	//radii:
	float WheelRadius[4];
	WheelRadius[0] = m_FrontWheelRadius;
	WheelRadius[1] = m_FrontWheelRadius;
	WheelRadius[2] = m_RearWheelRadius;
	WheelRadius[3] = m_RearWheelRadius;

	//angular velocities:
	float w[4];
	w[0] = m_w1;
	w[1] = m_w2;
	w[2] = m_w3;
	w[3] = m_w4;

	CVector LinVel = Rmat_inv * m_Velocity;
	CVector AngVel = Rmat_inv * m_AngularVelocity;

	float Mdrive[4];
	Mdrive[0] = m_M1;
	Mdrive[1] = m_M2;
	Mdrive[2] = m_M3;
	Mdrive[3] = m_M4;

	for(unsigned int i=0; i < 4; i++)
	{
		//make sure that the z axis is not flipped
		if(wz[i].z < 0.0)
			wz[i] = -wz[i];

		//x direction:
		CVector wx = wz[i].crossProduct(CVector(0,-1,0));

		//normal force:
		float Fnormal = 0.25 * Fy_tot; //temporary solution to normal forces

		//velocities:
		CVector Vw = (w[i] * WheelRadius[i]) * wz[i];
		CVector Vopp = LinVel + r[i].crossProduct(AngVel);
		CVector Vrel = Vopp + Vw; //The velocity of the tyre surface

		float vlong = -Vopp.dotProduct(wz[i]); //longitudinal velocity. Positive is forward
		float vlat =  Vopp.dotProduct(wx); //lateral velocity. Positive is right

		float slipratio = (w[i] * WheelRadius[i] - vlong) / (0.01 + fabs(vlong));

		//find the slip angle:
		float slipangle = atan2f(-vlat, vlong); //positive is counterclockwise

		if(fabs(vlat) < 1.0 && fabs(vlong) < 1.0) slipangle = 0.0; //TODO: put this in some magic formula

		//Now calculate a torque that depends on the slipangle:
		//TODO: correct translation of this torques
		addTorque(CVector(0, -mrotf * Fnormal * fabs(vlong) * sin(2*slipangle), 0));

		//The longitudinal force (traction + braking)
		//The lateral force (steering)
		//both normalised with respect to the normal force
		float Flong = tractionconstant * slipratio; //positive points forward
		float Flat = -cornerstiffness * vlat / (1.0 + 0.01 * fabs(vlong)); //positive points right

		//printf("Flong, Flat = %.3f, %.3f\n", Flong, Flat);

		//static friction:
		float fhor_abs = sqrt(Flong*Flong + Flat*Flat);
		if(fhor_abs > staticfric)
		{
			float factor = staticfric / fhor_abs;

			printf("static friction exceeded with factor %.3f\n", 1.0 / factor);

			Flong *= factor;
			Flat *= factor;
		}

		//printf("Flat %d = %.3f\n", i, Flat);

		//feedback to the net torque on the wheel:
		Mdrive[i] -= Fnormal * Flong * WheelRadius[i];

		//add forces to the wheel
		CVector F = Fnormal * (CVector(0,1,0) + Flong * -wz[i] + Flat * wx);

		F *= m_OrientationMatrix;
		r[i] *= m_OrientationMatrix;

		addForceAt(F, r[i]);
	}

	m_M1 = Mdrive[0];
	m_M2 = Mdrive[1];
	m_M3 = Mdrive[2];
	m_M4 = Mdrive[3];
}

CBinBuffer &CCar::getData(CBinBuffer &b) const
{
	CMovingObject::getData(b);

	//TODO: update this

	b += Uint8(m_Gear);
	b.addFloat16(m_MainAxisVelocity, 0.4);
	b.addFloat8(m_DesiredSteering, 0.008);
	b.addFloat8(m_gas, 0.008);

	return b;
}

bool CCar::setData(const CBinBuffer &b, unsigned int &pos)
{
	if(!CMovingObject::setData(b, pos)) return false;

	//TODO: update this

	m_Gear = b.getUint8(pos);
	m_MainAxisVelocity = b.getFloat16(pos, 0.4);
	m_DesiredSteering = b.getFloat8(pos, 0.008);
	m_gas = b.getFloat8(pos, 0.008);

	return true;
}
