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
	m_Ground = NULL;
}

CCar::~CCar()
{
}

bool CCar::load(const CString &filename, const CParamList &list)
{
	CMovingObject::load(filename, list);

	//initial state
	m_DesiredSteering = 0.0;
	m_xAngle = 0.0;
	m_zAngle = 0.0;
	m_Ground = NULL;

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
	m_CameraPos = cfile.getValue("body", "camerapos").toVector();
	m_cwA = cfile.getValue("body", "cwa").toFloat();
	m_RotationDamping = cfile.getValue("body", "rotationdamping").toFloat();

	//wheels
	CString frontwheelgeomfile = cfile.getValue("frontwheels", "geometry");
	CString rearwheelgeomfile = cfile.getValue("rearwheels", "geometry");
	float FrontWheelMu = cfile.getValue("frontwheels", "mu").toFloat();
	float RearWheelMu = cfile.getValue("rearwheels", "mu").toFloat();
	CVector FrontWheelNeutral = cfile.getValue("frontwheels", "position").toVector();
	CVector RearWheelNeutral = cfile.getValue("rearwheels", "position").toVector();
	m_FrontBrakeMax = cfile.getValue("frontwheels", "brakemax").toFloat();
	m_RearBrakeMax = cfile.getValue("rearwheels", "brakemax").toFloat();
	m_FrontSteerMax = cfile.getValue("frontwheels", "steermax").toFloat();
	m_RearSteerMax = -cfile.getValue("rearwheels", "steermax").toFloat();
	m_Engine.m_FrontTraction = cfile.getValue("frontwheels", "traction").toFloat();
	m_Engine.m_RearTraction = cfile.getValue("rearwheels", "traction").toFloat();
	m_FrontDownforce = cfile.getValue("frontwheels", "downforce").toFloat();
	m_RearDownforce = cfile.getValue("rearwheels", "downforce").toFloat();

	//driving pipeline
	m_Engine.m_EngineM = cfile.getValue("engine", "enginetorque").toFloat();
	m_Engine.m_MaxEngineW = cfile.getValue("engine", "maxenginespeed").toFloat();
	m_Engine.m_dMdw = 5*m_Engine.m_EngineM / m_Engine.m_MaxEngineW;
	m_Engine.m_GearRatios.push_back(cfile.getValue("engine", "gear0").toFloat());
	m_Engine.m_GearRatios.push_back(cfile.getValue("engine", "gear1").toFloat());
	m_Engine.m_GearRatios.push_back(cfile.getValue("engine", "gear2").toFloat());
	m_Engine.m_GearRatios.push_back(cfile.getValue("engine", "gear3").toFloat());
	m_Engine.m_GearRatios.push_back(cfile.getValue("engine", "gear4").toFloat());
	m_Engine.m_GearRatios.push_back(cfile.getValue("engine", "gear5").toFloat());
	m_Engine.m_GearRatios.push_back(cfile.getValue("engine", "gear6").toFloat());
	m_Engine.m_DifferentialRatio = cfile.getValue("engine", "differentialratio").toFloat();

	//Two sounds:
	m_Sounds.push_back(theWorld->loadObject(cfile.getValue("sound", "engine"), CParamList(), CDataObject::eSample));
	m_Sounds.push_back(theWorld->loadObject(cfile.getValue("sound", "skid"), CParamList(), CDataObject::eSample));

	//One texture:
	m_Textures.push_back(theWorld->loadObject(cfile.getValue("texture", "file"), CParamList(), CDataObject::eMaterial));

	//The input object: CCarInput instead of CMovObjInput
	delete m_InputData;
	m_InputData = new CCarInput;
	m_InputData->m_MovObjID = m_MovObjID;

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

	wheel1.m_Body = theWorld->loadObject(frontwheelgeomfile, plist, CDataObject::eBound);
	wheel2.m_Body = wheel1.m_Body;
	if(wheel1.m_Body < 0)
		printf("Error: frontwheel geometry %s was not loaded\n", frontwheelgeomfile.c_str());

	wheel3.m_Body = theWorld->loadObject(rearwheelgeomfile, plist, CDataObject::eBound);
	wheel4.m_Body = wheel3.m_Body;
	if(wheel3.m_Body < 0)
		printf("Error: rearwheel geometry %s was not loaded\n", rearwheelgeomfile.c_str());

	m_Bodies.push_back(body);
	m_Bodies.push_back(wheel1);
	m_Bodies.push_back(wheel2);
	m_Bodies.push_back(wheel3);
	m_Bodies.push_back(wheel4);

	//Size of the wheels
	m_Wheel[0].m_Radius = ((CBound *)theWorld->getObject(CDataObject::eBound, wheel1.m_Body))->m_CylinderRadius;
	m_Wheel[1].m_Radius = ((CBound *)theWorld->getObject(CDataObject::eBound, wheel2.m_Body))->m_CylinderRadius;
	m_Wheel[2].m_Radius = ((CBound *)theWorld->getObject(CDataObject::eBound, wheel3.m_Body))->m_CylinderRadius;
	m_Wheel[3].m_Radius = ((CBound *)theWorld->getObject(CDataObject::eBound, wheel4.m_Body))->m_CylinderRadius;

	//neutral position
	m_Wheel[0].m_NeutralPos = CVector(-FrontWheelNeutral.x, FrontWheelNeutral.y, FrontWheelNeutral.z);
	m_Wheel[1].m_NeutralPos = CVector( FrontWheelNeutral.x, FrontWheelNeutral.y, FrontWheelNeutral.z);
	m_Wheel[2].m_NeutralPos = CVector(-RearWheelNeutral.x,  RearWheelNeutral.y,  RearWheelNeutral.z);
	m_Wheel[3].m_NeutralPos = CVector( RearWheelNeutral.x,  RearWheelNeutral.y,  RearWheelNeutral.z);

	//Calculate the position above the ground:
	m_PositionAboveGround = 0.5 * (
		m_Wheel[0].m_Radius - m_Wheel[0].m_NeutralPos.y +
		m_Wheel[2].m_Radius - m_Wheel[2].m_NeutralPos.y);

	m_Wheel[0].m_Mu = FrontWheelMu;
	m_Wheel[1].m_Mu = FrontWheelMu;
	m_Wheel[2].m_Mu = RearWheelMu;
	m_Wheel[3].m_Mu = RearWheelMu;

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
	//Body orientation matrix
	CMatrix bodyOri;
	bodyOri.setRotation(CVector(m_xAngle, 0.0, m_zAngle));

	m_Bodies[0].m_Position = m_Position;
	m_Bodies[0].m_OrientationMatrix = bodyOri * m_OrientationMatrix;

	for(unsigned int i=0; i < 4; i++)
	{
		float steeringAngle = m_Wheel[i].m_DesiredSt;
		float rotationAngle = m_Wheel[i].m_a;
		if(i == 1 || i == 3) //the right wheels
		{
			steeringAngle += M_PI;
			rotationAngle = -rotationAngle;
		}

		//s = wheel steering matrix
		//r = wheel rotation matrix
		CMatrix s, r;
		s.rotY(steeringAngle);
		r.rotX(rotationAngle);

		//i+1 because body 0 is the car main body
		m_Bodies[i+1].m_Position = m_Position + m_OrientationMatrix * m_Wheel[i].m_NeutralPos;
		m_Bodies[i+1].m_OrientationMatrix = r * s * m_OrientationMatrix;
	}
}

void CCar::placeOnGround()
{
	if(m_Ground == NULL) return;

	//fprintf(stderr, "ground normal = %s\n", CString(m_Ground->nor).c_str());

	//The right rotation:
	
	CMatrix &Mnu = m_OrientationMatrix;
	CVector ynu;
	ynu.x = Mnu.Element(1,0);
	ynu.y = Mnu.Element(1,1);
	ynu.z = Mnu.Element(1,2);

	CVector ystr = m_Ground->nor;
	CVector cp = ystr.crossProduct(ynu);
	
	float abscp = cp.abs();
	if(abscp > 0.001) cp *= (asin(abscp) / abscp);

	CMatrix rotate;
	rotate.setRotation(cp);

	Mnu *= rotate;

	//The right position:

	CVector &pos = m_Position;
	float dobj = pos.dotProduct(m_Ground->nor) - m_Ground->d;
	pos -= (dobj - m_PositionAboveGround) * m_Ground->nor;
	
	//align the linear velocity:
	float vvert = m_Velocity.dotProduct(m_Ground->nor);
	if(vvert < 0.0)
	{
		m_Velocity -= vvert * m_Ground->nor;
	}

	//align the angular velocity
	m_AngularVelocity = m_AngularVelocity.component(m_Ground->nor);

	//placeBodies(); //not needed
}

void CCar::update(CPhysics *simulator, float dt)
{
	//Reset the skid volume to 0.0
	for(unsigned int i=0; i < 4; i++)
		m_Wheel[i].m_SkidVolume = 0.0;

	doSteering(dt);

	m_Ground = simulator->getGroundFace(m_Position);

	simulateGeneral(simulator, dt);
	if(m_Ground == NULL)
	{
		//fprintf(stderr, "ground plane NOT found\n");
		simulateAir(simulator, dt);
	}
	else
	{
		float vvert = m_Velocity.dotProduct(m_Ground->nor);
		float dobj = m_Bodies[0].m_Position.dotProduct(m_Ground->nor) - m_Ground->d;
		if(vvert > 0.1 || dobj > 1.1 * m_PositionAboveGround)
		{
			//fprintf(stderr, "ground plane far below\n");
			simulateAir(simulator, dt);
		}
		else
		{
			//fprintf(stderr, "ground plane found\n");
			placeOnGround();
			simulateGround(simulator, dt);
		}
	}


	//the wheels
	for(unsigned int i=0; i < 4; i++)
	{
		m_Wheel[i].m_a += m_Wheel[i].m_w * dt;
		if(m_Wheel[i].m_a > DBLPI) m_Wheel[i].m_a -= DBLPI;
		if(m_Wheel[i].m_a < 0.0) m_Wheel[i].m_a += DBLPI;
		m_Wheel[i].m_w += dt * m_Wheel[i].m_M * m_Wheel[i].m_Iinv_eff;
	}

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
	m_Engine.m_Gear = ((CCarInput *)m_InputData)->m_Gear;
	m_Engine.m_Gas = ((CCarInput *)m_InputData)->m_Forward;
	m_Engine.update(dt, m_Wheel[0].m_w, m_Wheel[1].m_w, m_Wheel[2].m_w, m_Wheel[3].m_w);

	//----------------------
	//Wheel simulation
	//----------------------
	updateWheelOrientation();
	updateWheelTorques();
	calculateNormalForces();
}

void CCar::simulateAir(CPhysics *simulator, float dt)
{
	m_xAngle = 0.0;
	m_zAngle = 0.0;
}

void CCar::simulateGround(CPhysics *simulator, float dt)
{
	applyWheelForces();

	CVector M = m_Mtot;
	M *= m_OrientationMatrix.transpose(); //in car coordinates
	
	if(fabs(m_xAngle) < 0.25)
		{m_xAngle += 0.05 * dt * M.x * m_InvMass;}
	else
		{m_xAngle = 0.9 * m_xAngle;}

	if(fabs(m_zAngle) < 0.25)
		{m_zAngle += 0.5 * dt * M.z * m_InvMass;}
	else
		{m_zAngle = 0.9 * m_zAngle;}
}

void CCar::addDownforce()
{
	const CMatrix &ori = m_Bodies[0].m_OrientationMatrix;
	
	CVector v = m_Velocity;

	v /= ori;

	if(v.z < 0.0) //moving forward
	{
		CVector frontpos(0.0, 0.0, m_Wheel[0].m_NeutralPos.z);
		CVector rearpos(0.0, 0.0, m_Wheel[2].m_NeutralPos.z);

		CVector frontF(0.0, -m_FrontDownforce*v.z*v.z, 0.0);
		CVector rearF(0.0, -m_RearDownforce*v.z*v.z, 0.0);

		frontpos *= ori;
		rearpos *= ori;
		frontF *= ori;
		rearF *= ori;

		addForceAt(frontF, frontpos);
		addForceAt(rearF, rearpos);
	}
}

void CCar::doSteering(float dt)
{
	CCarInput *input = (CCarInput *)m_InputData;
	float steer = input->m_Right;

	float factor = exp(-0.5*dt);
	if(fabsf(steer) < fabsf(m_DesiredSteering))
		factor = exp(-25.0*dt);

	m_DesiredSteering = factor * m_DesiredSteering + (1.0-factor) * steer;

	//if the wheels were in the middle of the car
	float desiredfront = m_FrontSteerMax * m_DesiredSteering;
	float desiredrear = m_RearSteerMax * m_DesiredSteering;

	//desired angles
	//important: default to zero
	for(unsigned int i=0; i < 4; i++)
		m_Wheel[i].m_DesiredSt = 0.0;

	if(fabs(desiredfront) > 0.0001 || fabs(desiredrear) > 0.0001)
	{
		//This correction is actually better than that
		//is possible in a real car
		float lth = m_Wheel[2].m_NeutralPos.z  - m_Wheel[0].m_NeutralPos.z;
		float halfwthf = m_Wheel[1].m_NeutralPos.x;
		float halfwthr = m_Wheel[3].m_NeutralPos.x;
		float tanaf = tanf(desiredfront);
		float tanar = tanf(-desiredrear);
		float yf = lth * tanaf / (tanar + tanaf);
		float yr = lth - yf;
		float radius = 0.0;
		if(fabs(tanaf) > 0.0001)
			{radius = 0.5*lth / tanaf;} //yf / tanaf}
		else
			{radius = yr / tanar;}

		m_Wheel[0].m_DesiredSt = atanf(yf / (radius + halfwthf));
		m_Wheel[1].m_DesiredSt = atanf(yf / (radius - halfwthf));
		m_Wheel[2].m_DesiredSt = -atanf(yr / (radius + halfwthr));
		m_Wheel[3].m_DesiredSt = -atanf(yr / (radius - halfwthr));
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
	s1.rotY(m_Wheel[0].m_DesiredSt);
	s2.rotY(m_Wheel[1].m_DesiredSt);
	s3.rotY(m_Wheel[2].m_DesiredSt);
	s4.rotY(m_Wheel[3].m_DesiredSt);

	m_Wheel[0].m_Z = s1 * zaxis;
	m_Wheel[1].m_Z = s2 * zaxis;
	m_Wheel[2].m_Z = s3 * zaxis;
	m_Wheel[3].m_Z = s4 * zaxis;
}

void CCar::updateWheelTorques()      //engine + brakes
{
	CCarInput *input = (CCarInput *)m_InputData;
	float frontbrake = m_FrontBrakeMax * input->m_Backward;
	float rearbrake = m_RearBrakeMax * input->m_Backward;

	m_Wheel[0].m_M = 0.5 * (m_Engine.getWheelM(0) - frontbrake * m_Wheel[0].m_w);
	m_Wheel[1].m_M = 0.5 * (m_Engine.getWheelM(1) - frontbrake * m_Wheel[1].m_w);
	m_Wheel[2].m_M = 0.5 * (m_Engine.getWheelM(2) - rearbrake * m_Wheel[2].m_w);
	m_Wheel[3].m_M = 0.5 * (m_Engine.getWheelM(3) - rearbrake * m_Wheel[3].m_w);

	//adding the opposite torques to the body
	for(unsigned int i=0; i < 3; i++)
	{
		CVector wz = m_Wheel[i].m_Z;

		//make sure that the z axis is not flipped
		if(i == 1 || i == 3)
			wz = -wz;

		//x direction:
		CVector wx = wz.crossProduct(CVector(0,-1,0));

		addTorque(-m_Wheel[i].m_M * wx);
	}
}

void CCar::calculateNormalForces()
{
	CMatrix Rmat_inv = m_OrientationMatrix.transpose();

	//external forces
	CVector Fext = Rmat_inv * m_Ftot;
	float Fy_tot = -Fext.y;

	CMatrix bodyOri;
	bodyOri.setRotation(CVector(m_xAngle, 0.0, m_zAngle));

	float zfr = m_Wheel[0].m_NeutralPos.z, zba = m_Wheel[2].m_NeutralPos.z;
	float length = zba - zfr;

	//neutral weight distribution
	float wf[4];
	wf[0] = wf[1] =  0.5 * zba / length;
	wf[2] = wf[3] = -0.5 * zfr / length;

	for(unsigned int i=0; i < 4; i++)
	{
		//neutral position of the wheel:
		CVector r0 = m_Wheel[i].m_NeutralPos;

		//real position of the body at the wheel positions:
		CVector r = bodyOri * r0;

		//y difference:
		float dy = r.y - r0.y;

		m_Wheel[i].m_Fnormal = wf[i] * Fy_tot - 100000.0 * dy; //TODO: variable k
		if(m_Wheel[i].m_Fnormal < 0.0) m_Wheel[i].m_Fnormal = 0.0; //no "glue" forces
	}
}

void CCar::applyWheelForces()
{
	float muGround = 1.0;
	if(m_Ground != NULL && m_Ground->material != NULL)
		muGround = m_Ground->material->m_Mu;

	//This function mainly works in the car coordinate system
	CMatrix Rmat_inv = m_OrientationMatrix.transpose();

	CVector LinVel = Rmat_inv * m_Velocity;
	CVector AngVel = Rmat_inv * m_AngularVelocity;

	for(unsigned int i=0; i < 4; i++)
	{
		//position of the wheel:
		CVector r = m_Wheel[i].m_NeutralPos;

		//z axis:
		CVector wz = m_Wheel[i].m_Z;

		wz *= Rmat_inv;

		//make sure that the z axis is not flipped
		if(wz.z < 0.0)
			wz = -wz;

		//x direction:
		CVector wx = wz.crossProduct(CVector(0,-1,0));

		//velocity of the center of the wheel:
		CVector Vcent = LinVel + r.crossProduct(AngVel);

		float vlong = Vcent.dotProduct(wz); //longitudinal velocity. Positive is backward
		float vlat =  Vcent.dotProduct(wx); //lateral velocity. Positive is right

		float Mwheel = 0.0;
		CVector Fwheel = m_Wheel[i].getGroundForce(Mwheel, vlong, vlat, m_Wheel[i].m_Mu * muGround);

		//feedback to the net torque on the wheel:
		m_Wheel[i].m_M += Fwheel.z * m_Wheel[i].m_Radius;

		//add forces to the wheel
		Fwheel *= m_OrientationMatrix;
		r += CVector(0.0, -m_Wheel[i].m_Radius, 0.0); //forces attach on surface, not on middle
		r *= m_OrientationMatrix;
		addForceAt(Fwheel, r);
	}
}

CBinBuffer &CCar::getData(CBinBuffer &b) const
{
	CMovingObject::getData(b);

	//TODO: update this

	b += Uint8(m_Engine.m_Gear);
	b.addFloat16(m_Engine.m_MainAxisW, 0.4);
	b.addFloat8(m_DesiredSteering, 0.008);
	b.addFloat8(m_Engine.m_Gas, 0.008);

	return b;
}

bool CCar::setData(const CBinBuffer &b, unsigned int &pos)
{
	if(!CMovingObject::setData(b, pos)) return false;

	//TODO: update this

	m_Engine.m_Gear = b.getUint8(pos);
	m_Engine.m_MainAxisW = b.getFloat16(pos, 0.4);
	m_DesiredSteering = b.getFloat8(pos, 0.008);
	m_Engine.m_Gas = b.getFloat8(pos, 0.008);

	return true;
}
