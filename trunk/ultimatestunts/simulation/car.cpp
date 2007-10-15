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

#include "pi.h"
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
	m_Ground.nor = CVector(0,0,0);
	m_SimState = eFlying;
}

CCar::~CCar()
{
}

bool CCar::load(const CString &filename, const CParamList &list)
{
	CMovingObject::load(filename, list);

	//initial state
	m_DesiredSteering = 0.0;
	//m_xAngle = 0.0;
	//m_zAngle = 0.0;
	//m_BodyHeight = 0.0;

	CDataFile dfile(getFilename());
	CLConfig cfile(dfile.useExtern());
	//TODO: make a way to find out if this file exists
	//and return false if it doesn't

	//Description
	m_CarName = cfile.getValue("description", "fullname");

	//Mass + moment of inertia
	m_InvMass = 1.0 / cfile.getValue("body", "mass").toFloat();
	m_BodySize = cfile.getValue("body", "size").toVector();
	m_InvInertia.setElement(0, 0, 1.0 / (m_BodySize.y*m_BodySize.y + m_BodySize.z*m_BodySize.z));
	m_InvInertia.setElement(1, 1, 1.0 / (m_BodySize.x*m_BodySize.x + m_BodySize.z*m_BodySize.z));
	m_InvInertia.setElement(2, 2, 1.0 / (m_BodySize.x*m_BodySize.x + m_BodySize.y*m_BodySize.y));
	m_InvInertia *= 12.0 * m_InvMass;
	m_CenterOfMass = cfile.getValue("body", "centerofmass").toVector();

	//body
	CString bodygeomfile = cfile.getValue("body", "geometry");
	m_CameraPos = cfile.getValue("body", "camerapos").toVector();
	m_cwA = cfile.getValue("body", "cwa").toFloat();
	m_RotationDamping = cfile.getValue("body", "rotationdamping").toFloat();

	//wheels
	CString frontwheelgeomfile        = cfile.getValue("frontwheels", "geometry");
	CString rearwheelgeomfile         = cfile.getValue("rearwheels", "geometry");
	m_FrontSteerMax                   = cfile.getValue("frontwheels", "steermax").toFloat();
	m_RearSteerMax                    = -cfile.getValue("rearwheels", "steermax").toFloat();
	m_Engine.m_FrontTraction          = cfile.getValue("frontwheels", "traction").toFloat();
	m_Engine.m_RearTraction           = cfile.getValue("rearwheels", "traction").toFloat();
	m_FrontDownforce                  = cfile.getValue("frontwheels", "downforce").toFloat();
	m_RearDownforce                   = cfile.getValue("rearwheels", "downforce").toFloat();

	//Engine torque curve:
	m_Engine.m_M0     = cfile.getValue("engine", "zerotorque").toFloat();
	m_Engine.m_Mmax   = cfile.getValue("engine", "maxtorque").toFloat();
	m_Engine.m_w_Mmax = cfile.getValue("engine", "w_maxtorque").toFloat();
	m_Engine.m_Pmax   = cfile.getValue("engine", "maxpower").toFloat();
	m_Engine.m_w_Pmax = cfile.getValue("engine", "w_maxpower").toFloat();
	m_Engine.m_w_Zero = cfile.getValue("engine", "w_zero").toFloat();

	//driving pipeline
	float gear0 = cfile.getValue("engine", "gear0").toFloat();
	float gear1 = cfile.getValue("engine", "gear1").toFloat();
	float gear2 = cfile.getValue("engine", "gear2").toFloat();
	float gear3 = cfile.getValue("engine", "gear3").toFloat();
	float gear4 = cfile.getValue("engine", "gear4").toFloat();
	float gear5 = cfile.getValue("engine", "gear5").toFloat();
	float gear6 = cfile.getValue("engine", "gear6").toFloat();
	m_Engine.m_GearRatios.push_back(gear0);
	m_Engine.m_GearRatios.push_back(gear1);
	if(gear2 > 0.00001) m_Engine.m_GearRatios.push_back(gear2);
	if(gear3 > 0.00001) m_Engine.m_GearRatios.push_back(gear3);
	if(gear4 > 0.00001) m_Engine.m_GearRatios.push_back(gear4);
	if(gear5 > 0.00001) m_Engine.m_GearRatios.push_back(gear5);
	if(gear6 > 0.00001) m_Engine.m_GearRatios.push_back(gear6);

	m_Engine.m_DifferentialRatio = cfile.getValue("engine", "differentialratio").toFloat();

	//The sounds:
	m_Sounds.push_back(theWorld->loadObject(cfile.getValue("sound", "engine"), CParamList(), CDataObject::eSample));
	{
		CString baseRPS = cfile.getValue("sound", "enginerps");
		m_EngineSoundBaseRPS = 388.0;
		if(baseRPS != "")
			m_EngineSoundBaseRPS = baseRPS.toFloat();
	}

	//One texture:
	m_Textures.push_back(theWorld->loadObject(cfile.getValue("skin", "texture"), CParamList(), CDataObject::eMaterial));

	//Dashboard info:
	m_Dashboard.background_tex = cfile.getValue("dashboard", "background_tex");
	m_Dashboard.background_hth = cfile.getValue("dashboard", "background_hth").toFloat();

	m_Dashboard.crash_background_tex = cfile.getValue("dashboard", "crash_background_tex");
	m_Dashboard.crash_tex            = cfile.getValue("dashboard", "crash_tex");

	m_Dashboard.steer_tex = cfile.getValue("dashboard", "steer_tex");
	m_Dashboard.steer_pos = cfile.getValue("dashboard", "steer_pos").toVector();
	m_Dashboard.steer_rad = cfile.getValue("dashboard", "steer_rad").toFloat();
	m_Dashboard.steer_ang = cfile.getValue("dashboard", "steer_ang").toFloat();

	m_Dashboard.analog_vel_tex = cfile.getValue("dashboard", "analog_vel_tex");
	m_Dashboard.analog_vel_pos = cfile.getValue("dashboard", "analog_vel_pos").toVector();
	m_Dashboard.analog_vel_rad = cfile.getValue("dashboard", "analog_vel_rad").toFloat();
	m_Dashboard.analog_vel_an0 = cfile.getValue("dashboard", "analog_vel_an0").toFloat();
	m_Dashboard.analog_vel_an1 = cfile.getValue("dashboard", "analog_vel_an1").toFloat();
	m_Dashboard.analog_vel_max = cfile.getValue("dashboard", "analog_vel_max").toFloat();

	m_Dashboard.analog_rpm_tex = cfile.getValue("dashboard", "analog_rpm_tex");
	m_Dashboard.analog_rpm_pos = cfile.getValue("dashboard", "analog_rpm_pos").toVector();
	m_Dashboard.analog_rpm_rad = cfile.getValue("dashboard", "analog_rpm_rad").toFloat();
	m_Dashboard.analog_rpm_an0 = cfile.getValue("dashboard", "analog_rpm_an0").toFloat();
	m_Dashboard.analog_rpm_an1 = cfile.getValue("dashboard", "analog_rpm_an1").toFloat();
	m_Dashboard.analog_rpm_max = cfile.getValue("dashboard", "analog_rpm_max").toFloat();

	m_Dashboard.digital_vel_pos = cfile.getValue("dashboard", "digital_vel_pos").toVector();
	m_Dashboard.digital_vel_hth = cfile.getValue("dashboard", "digital_vel_hth").toFloat();
	m_Dashboard.digital_vel_wth = cfile.getValue("dashboard", "digital_vel_wth").toFloat();
	m_Dashboard.digital_rpm_pos = cfile.getValue("dashboard", "digital_rpm_pos").toVector();
	m_Dashboard.digital_rpm_hth = cfile.getValue("dashboard", "digital_rpm_hth").toFloat();
	m_Dashboard.digital_rpm_wth = cfile.getValue("dashboard", "digital_rpm_wth").toFloat();
	m_Dashboard.songtitle_pos = cfile.getValue("dashboard", "songtitle_pos").toVector();
	m_Dashboard.songtitle_hth = cfile.getValue("dashboard", "songtitle_hth").toFloat();
	m_Dashboard.songtitle_wth = cfile.getValue("dashboard", "songtitle_wth").toFloat();

	m_SteerSpeedOut = cfile.getValue("controls", "steerspeed_out").toFloat();
	m_SteerSpeedIn  = cfile.getValue("controls", "steerspeed_in").toFloat();
	m_SteerSpeed_v_factor  = cfile.getValue("controls", "steerspeed_v_factor").toFloat();

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

		p.name = "color";
		p.value = list.getValue("color", cfile.getValue("skin", "defaultcolor"));
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

	//Important: do this AFTER setting m_Radius:
	m_Wheel[0].load(cfile, "frontwheels");
	m_Wheel[1].load(cfile, "frontwheels");
	m_Wheel[2].load(cfile, "rearwheels");
	m_Wheel[3].load(cfile, "rearwheels");

	//Correcting the neutral position
	m_Wheel[0].m_NeutralPos.x = -m_Wheel[0].m_NeutralPos.x;
	m_Wheel[2].m_NeutralPos.x = -m_Wheel[2].m_NeutralPos.x;

	for(unsigned int i=0; i<4; i++)
		m_Wheel[i].m_NeutralPos -= m_CenterOfMass; //save as relative to center of mass

	//Calculate the position above the ground:
	m_PositionAboveGround = 0.5 * (
		m_Wheel[0].m_Radius - m_Wheel[0].m_NeutralPos.y +
		m_Wheel[2].m_Radius - m_Wheel[2].m_NeutralPos.y);

	//Setting the initial positions
	m_Position = CVector(0,0,0);
	m_OrientationMatrix = CMatrix();
	resetBodyPositions();

	return true;
}

void CCar::unload()
{
	//TODO
	CMovingObject::unload();
}

void CCar::resetBodyPositions()
{
	placeBodies();
}

void CCar::placeBodies()
{
	m_Bodies[0].m_Position = m_Position -
		m_OrientationMatrix * m_CenterOfMass;

	m_Bodies[0].m_OrientationMatrix = m_OrientationMatrix;

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
		m_Bodies[i+1].m_Position = m_Position + m_OrientationMatrix *
			(m_Wheel[i].m_NeutralPos + CVector(0,m_Wheel[i].m_Height,0));
		m_Bodies[i+1].m_OrientationMatrix = r * s * m_OrientationMatrix;
	}
}

void CCar::determineGroundPlane(CPhysics *simulator)
{
	vector<CCollisionFace> wheelGround;
	vector<CVector> contactPoint;

	//Debugging:
	//m_Ground.nor = CVector(0,1,0);
	//m_Ground.d = 0.0;
	//return;

	//fprintf(stderr, "\n\ndetermineGroundPlane\n");

	//get the ground faces for the wheels
	for(unsigned int i=0; i < 4; i++)
	{
		CVector pos = m_OrientationMatrix * (m_Wheel[i].m_NeutralPos + CVector(0,m_Wheel[i].m_Radius,0));
		//fprintf(stderr, "Wheel %d height: %.3f\n", i, (pos + m_Position).y);

		const CCollisionFace * theFace = theWorld->m_Detector.getGroundFace(pos + m_Position);
		if(theFace != NULL)
		{
			//In absolute coordinates:
			m_Wheel[i].m_Ground = *theFace;

			//relative to the car center:
			CCollisionFace cf = *theFace;
			cf.d -= m_Position.dotProduct(cf.nor);
			float dpos = pos.dotProduct(cf.nor);

			//check if it's close enough to the wheel
			if(dpos - cf.d > 5.0 * m_Wheel[i].m_Radius)
				continue;

			//fprintf(stderr, "  -> on the ground\n");

			//contact position
			pos += (cf.d - dpos) * cf.nor;

			contactPoint.push_back(pos);
			wheelGround.push_back(cf);
		}
	}

	//printf("%d\n", wheelGround.size());

	switch(wheelGround.size())
	{
	case 4:
	{
		//a plane through each combination of 3 wheels
		CCollisionFace planes[4];

		for(unsigned int i=0; i < 4; i++)
		{
			int indices[3]; //the 3 points

			//only the points that are not i
			int count = 0;
			for(unsigned int j=0; j < 4; j++)
				if(j != i)
				{
					indices[count] = j;
					count++;
				}

			CVector line1 = contactPoint[indices[1]] - contactPoint[indices[0]];
			CVector line2 = contactPoint[indices[2]] - contactPoint[indices[0]];
			
			planes[i].nor = line1.crossProduct(line2).normal();

			CVector norsum =
				wheelGround[indices[0]].nor +
				wheelGround[indices[1]].nor +
				wheelGround[indices[2]].nor;

			if(norsum.dotProduct(planes[i].nor) < 0.0) //the real normals point to the other side
				planes[i].nor = -planes[i].nor;

			planes[i].d = contactPoint[indices[0]].dotProduct(planes[i].nor);
		}

		//now choose a plane. The other wheel should be as much above its own plane as possible
		//so its contact point should be as much below the chosen plane as possible

		//to make the simulation more continuous, we don't choose, but
		//we use the height as a weight

		m_Ground.nor = CVector(0,0,0);
		m_Ground.d = 0.0;

		float weight[4];
		for(unsigned int i=0; i < 4; i++)
		{
			float height = contactPoint[i].dotProduct(planes[i].nor) - planes[i].d;
			if(height < 0.0)
				{weight[i] = 1.0;}
			else
				{weight[i] = 0.1;}
			//weight[i] =  exp(-height); //always positive, >> 0 for height = 0

			m_Ground.nor += weight[i] * planes[i].nor;
			m_Ground.d += weight[i] * planes[i].d;
		}
		float weightsum = weight[0] + weight[1] + weight[2] + weight[3];
		if(fabs(weightsum) > 0.01)
		{
			m_Ground.d /= weightsum;
		}
		else
		{
			m_Ground.d = 0.0;
		}
		m_Ground.nor.normalise();

		float norcomp0, norcomp1, norcomp2, norcomp3;
		norcomp0 = wheelGround[0].nor.dotProduct(m_Ground.nor);
		norcomp1 = wheelGround[1].nor.dotProduct(m_Ground.nor);
		norcomp2 = wheelGround[2].nor.dotProduct(m_Ground.nor);
		norcomp3 = wheelGround[3].nor.dotProduct(m_Ground.nor);

		if(norcomp0 > norcomp1 && norcomp0 > norcomp2 && norcomp0 > norcomp3)
		{
			m_Ground.material = wheelGround[0].material;
			m_Ground.isSurface = wheelGround[0].isSurface;
			m_Ground.isWater = wheelGround[0].isWater;
		}
		else if(norcomp1 > norcomp2 && norcomp1 > norcomp3)
		{
			m_Ground.material = wheelGround[1].material;
			m_Ground.isSurface = wheelGround[1].isSurface;
			m_Ground.isWater = wheelGround[1].isWater;
		}
		else if(norcomp2 > norcomp3)
		{
			m_Ground.material = wheelGround[2].material;
			m_Ground.isSurface = wheelGround[2].isSurface;
			m_Ground.isWater = wheelGround[2].isWater;
		}
		else
		{
			m_Ground.material = wheelGround[3].material;
			m_Ground.isSurface = wheelGround[3].isSurface;
			m_Ground.isWater = wheelGround[3].isWater;
		}

		break;
	}
	case 3:
	{
		CVector line1 = contactPoint[1] - contactPoint[0];
		CVector line2 = contactPoint[2] - contactPoint[0];

		m_Ground.nor = line1.crossProduct(line2).normal();

		CVector norsum = wheelGround[0].nor + wheelGround[1].nor + wheelGround[2].nor;

		if(norsum.dotProduct(m_Ground.nor) < 0.0) //the real normals point to the other side
			m_Ground.nor = -m_Ground.nor;

		m_Ground.d = contactPoint[0].dotProduct(m_Ground.nor);

		float norcomp0, norcomp1, norcomp2;
		norcomp0 = wheelGround[0].nor.dotProduct(m_Ground.nor);
		norcomp1 = wheelGround[1].nor.dotProduct(m_Ground.nor);
		norcomp2 = wheelGround[2].nor.dotProduct(m_Ground.nor);

		if(norcomp0 > norcomp1 && norcomp0 > norcomp2)
		{
			m_Ground.material = wheelGround[0].material;
			m_Ground.isSurface = wheelGround[0].isSurface;
			m_Ground.isWater = wheelGround[0].isWater;
		}
		else if(norcomp1 > norcomp2)
		{
			m_Ground.material = wheelGround[1].material;
			m_Ground.isSurface = wheelGround[1].isSurface;
			m_Ground.isWater = wheelGround[1].isWater;
		}
		else
		{
			m_Ground.material = wheelGround[2].material;
			m_Ground.isSurface = wheelGround[2].isSurface;
			m_Ground.isWater = wheelGround[2].isWater;
		}

		break;
	}
	case 2:
	{
		CVector line = (contactPoint[1] - contactPoint[0]).normal();

		CVector nor0_comp = wheelGround[0].nor - wheelGround[0].nor.component(line);
		CVector nor1_comp = wheelGround[1].nor - wheelGround[1].nor.component(line);

		m_Ground.nor = (nor0_comp + nor1_comp).normal();
		m_Ground.d = contactPoint[0].dotProduct(m_Ground.nor);

		if(nor0_comp.abs2() > nor1_comp.abs2())
		{
			m_Ground.material = wheelGround[0].material;
			m_Ground.isSurface = wheelGround[0].isSurface;
			m_Ground.isWater = wheelGround[0].isWater;
		}
		else
		{
			m_Ground.material = wheelGround[1].material;
			m_Ground.isSurface = wheelGround[1].isSurface;
			m_Ground.isWater = wheelGround[1].isWater;
		}

		break;
	}
	case 1:
	{
		m_Ground = wheelGround[0];
		break;
	}
	case 0:
	{
		m_Ground.nor = CVector(0,0,0);
		break;
	}
	}

	//back to absolute coordinates:
	m_Ground.d += m_Position.dotProduct(m_Ground.nor);
}

void CCar::placeOnGround()
{
	//Place wheels on ground
	for(unsigned int i=0; i < 4; i++)
	{
		const CCollisionFace &ground = m_Wheel[i].m_Ground;
		float ndoty = ground.nor.dotProduct(m_OrientationMatrix * CVector(0,1,0));

		if(ndoty < 0.1)
		{
			m_Wheel[i].m_Height = 0.0;
			m_Wheel[i].m_dHeight = 0.0;
			continue;
		}

		float ndoty_inv = 1.0 / ndoty;

		CVector neutral_rel = m_OrientationMatrix * m_Wheel[i].m_NeutralPos;
		CVector neutral = m_Position + neutral_rel;
		m_Wheel[i].m_Height = ndoty_inv * (ground.d + m_Wheel[i].m_Radius - ground.nor.dotProduct(neutral));

		//Assuming that ndoty is more or less constant:
		m_Wheel[i].m_dHeight = -ndoty_inv * ground.nor.dotProduct(
			m_Velocity - m_AngularVelocity.crossProduct(neutral_rel));

		if(m_Wheel[i].m_Height < 0.0)
		{
			m_Wheel[i].m_Height = 0.0;
			m_Wheel[i].m_dHeight = 0.0;
		}
	}
}

void CCar::landOnGround()
{
	/*
	All models are wrong, but some are useful.
	G.E.P. Box
	*/

	if(m_Ground.nor.abs2() < 0.25) return;

	//The right rotation:
	CMatrix &Mnu = m_OrientationMatrix;
	CVector ynu;
	ynu.x = Mnu.Element(1,0);
	ynu.y = Mnu.Element(1,1);
	ynu.z = Mnu.Element(1,2);

	CVector ystr = m_Ground.nor;
	CVector cp = ystr.crossProduct(ynu);
	
	float abscp = cp.abs();
	if(abscp > 0.001) cp *= (asin(abscp) / abscp);

	CMatrix rotate;
	rotate.setRotation(cp);

	Mnu *= rotate;

	//align the angular velocity
	m_AngularVelocity = m_AngularVelocity.component(m_Ground.nor);

	//The right position:
	CVector &pos = m_Position;
	float dobj = pos.dotProduct(m_Ground.nor) - m_Ground.d;
	pos -= (dobj - m_PositionAboveGround) * m_Ground.nor;

	//align the linear velocity:
	float vvert = m_Velocity.dotProduct(m_Ground.nor);
	if(vvert < 0.0)
	{
		m_Velocity -= vvert * m_Ground.nor;
	}

	printf("Landed\n");
}

void CCar::fixFlyingOrientation()
{
	//Goal: align the nose more or less with the velocity vector
	//More precise: the vertical y-axis is made perpendicular to
	//the velocity vector

	//don't do it at low speeds
	if(m_Velocity.abs2() < 100.0) return;

	//The Y-axis orientation:
	CVector yNow = m_OrientationMatrix * CVector(0,1,0);

	//The velocity vector, normalised:
	CVector v = m_Velocity.normal();

	//The component of the Y-axis perpendicular to v, normalised:
	CVector yNew = (yNow - yNow.component(v)).normal();
	if(yNew.dotProduct(yNow) < 0.0) yNew = -yNew;

	//The rotation axis
	CVector rAxis = yNew.crossProduct(yNow);
	float rAxisSize = rAxis.abs();

	if(rAxisSize < 0.001) return; //rotation is not needed; prevent division by zero

	float angle = fabsf(asin(rAxisSize));

	//Don't rotate too much:
	angle *= 0.5; //some kind of under-compensation
	if(angle >  0.01) angle =  0.01;

	rAxis *= (angle / rAxisSize);

	CMatrix rot;
	rot.setRotation(rAxis);
	m_OrientationMatrix *= rot;

	m_AngularVelocity -= m_AngularVelocity.component(rAxis.normal());
}

void CCar::update(CPhysics *simulator, float dt)
{
	//Reset the skid volume to 0.0
	//Reset the torque on the wheel to 0.0
	for(unsigned int i=0; i < 4; i++)
	{
		m_Wheel[i].m_SkidVolume = 0.0;
		m_Wheel[i].m_M = 0.0;
	}

	//Override input on crash
	if(m_RuleStatus.state == CCarRuleStatus::eCrashed)
	{
		m_InputData->m_Up = 0.0;
		m_InputData->m_Forward = 0.0;
		m_InputData->m_Backward = 1.0; //This is always the brake, even in reverse gear
		m_InputData->m_Right = 0.0;
	}

	//Wheel stuff
	doSteering(dt);

	simulateGeneral(simulator, dt);

	determineGroundPlane(simulator);
	if(m_Ground.nor.abs2() < 0.25)
	{
		//fprintf(stderr, "ground plane NOT found\n");
		simulateAir(simulator, dt);
	}
	else
	{
		float vvert = m_Velocity.dotProduct(m_Ground.nor);
		float dobj = m_Bodies[0].m_Position.dotProduct(m_Ground.nor) - m_Ground.d;
		unsigned numFloatingWheels = 0;
		for(unsigned int i=0; i<4; i++)
			if(m_Wheel[i].m_Height < 0.001) numFloatingWheels++;

		if(vvert > 0.1 && numFloatingWheels == 4)
		{
			//fprintf(stderr, "vertical speed pointing away from gound\n");
			//printf("Airborne!\n");
			simulateAir(simulator, dt);
		}
		else if(dobj > 10.0)
		{
			//fprintf(stderr, "ground plane far below\n");
			//printf("Airborne!\n");
			simulateAir(simulator, dt);
		}
		else
		{
			//fprintf(stderr, "ground plane found\n");
			simulateGround(simulator, dt);
		}
	}

	//integration step for the wheels
	for(unsigned int i=0; i < 4; i++)
	{
		m_Wheel[i].m_a += m_Wheel[i].m_w * dt;
		if(m_Wheel[i].m_a > DBLPI) m_Wheel[i].m_a -= DBLPI;
		if(m_Wheel[i].m_a < 0.0) m_Wheel[i].m_a += DBLPI;
		m_Wheel[i].m_w += dt * m_Wheel[i].m_M * m_Wheel[i].m_Iinv_eff;
	}

	//----------------------
	//Integration step
	//----------------------
	CMovingObject::update(simulator, dt);
}

void CCar::correctCollisions()
{
	for(unsigned int c=0; c < m_SimCollisions.size(); c++)
	{
		CCollisionData col = m_SimCollisions[c];

		//First do a crash test
		if(col.getRadVel() > 10.0) //10 m/s = 36 km/h
		{
			m_RuleStatus.crash();
			m_SimCollisions[c].fatal = true;
		}

		//Then do position correction
		CVector dr = col.nor * col.depth;

		//no correction towards the ground
		if(m_Ground.nor.abs2() > 0.25)
		{
			if(col.nor.dotProduct(m_Ground.nor) < -0.9) continue;

			CVector newcolnor = (col.nor - col.nor.component(m_Ground.nor)).normal();
			
			float dotpr = newcolnor.dotProduct(col.nor);
			if(fabs(dotpr) < 0.001) continue;

			dr = newcolnor * (col.depth / dotpr);
		}

		//correct the position
		m_Position += dr;

		//set the collision velocity (relative to vmean) to zero
		m_Velocity -= col.vmean;
		float radcomp = m_Velocity.dotProduct(col.nor);
		if(radcomp < 0.0)
			m_Velocity -= radcomp * col.nor;
		m_Velocity += col.vmean;
	}
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
	if(m_Engine.m_Gear >= m_Engine.m_GearRatios.size())
		m_Engine.m_Gear = m_Engine.m_GearRatios.size() - 1;
	m_Engine.m_Gas = ((CCarInput *)m_InputData)->m_Forward;
	m_Engine.update(dt, m_Wheel[0].m_w, m_Wheel[1].m_w, m_Wheel[2].m_w, m_Wheel[3].m_w);

	//----------------------
	//Wheel simulation
	//----------------------
	updateWheelOrientation();
	updateWheelTorques();
}

void CCar::simulateAir(CPhysics *simulator, float dt)
{
	//printf("Flying\n");
	m_SimState = eFlying;
	for(unsigned int i=0; i<4; i++)
	{
		m_Wheel[i].m_Fnormal = 0.0;
		m_Wheel[i].m_Height = 0.0;
		m_Wheel[i].m_dHeight = 0.0;
	}

	fixFlyingOrientation();
}

void CCar::simulateGround(CPhysics *simulator, float dt)
{
	if(m_SimState == eFlying)
		landOnGround();
	m_SimState = eRiding;
	//printf("Riding\n");

	placeOnGround(); //Places wheels on ground

	calculateNormalForces();
	applyWheelForces();
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

	//the speed of steering
	float steerspeed = m_SteerSpeedOut;
	if((m_DesiredSteering < 0.0 && steer > m_DesiredSteering) ||
		(m_DesiredSteering > 0.0 && steer < m_DesiredSteering)) 
		steerspeed = m_SteerSpeedIn; //steer back to neutral is faster

	//faster velocity -> slower steering
	steerspeed /= 1.0 + m_SteerSpeed_v_factor * m_Velocity.abs();


	//linear function:
	float steerchange = copysign(steerspeed * dt, steer - m_DesiredSteering);
	m_DesiredSteering += steerchange;
	if(m_DesiredSteering < -1.0) m_DesiredSteering = -1.0;
	if(m_DesiredSteering >  1.0) m_DesiredSteering =  1.0;

	//fixing straight road driving:
	if(fabsf(steer) < 0.05 && fabsf(m_DesiredSteering) < 0.05)
		m_DesiredSteering = 0.0;

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
	CVector
		z1(-sin(m_Wheel[0].m_DesiredSt), 0.0, cos(m_Wheel[0].m_DesiredSt)),
		z2(-sin(m_Wheel[1].m_DesiredSt), 0.0, cos(m_Wheel[1].m_DesiredSt)),
		z3(-sin(m_Wheel[2].m_DesiredSt), 0.0, cos(m_Wheel[2].m_DesiredSt)),
		z4(-sin(m_Wheel[3].m_DesiredSt), 0.0, cos(m_Wheel[3].m_DesiredSt));

	m_Wheel[0].m_Z = m_OrientationMatrix * z1;
	m_Wheel[1].m_Z = m_OrientationMatrix * z2;
	m_Wheel[2].m_Z = m_OrientationMatrix * z3;
	m_Wheel[3].m_Z = m_OrientationMatrix * z4;
}

void CCar::updateWheelTorques() //engine + brakes
{
	CCarInput *input = (CCarInput *)m_InputData;

	m_Wheel[0].m_M += m_Engine.getWheelM(0) - m_Wheel[0].getBrakeTorque(input->m_Backward);
	m_Wheel[1].m_M += m_Engine.getWheelM(1) - m_Wheel[1].getBrakeTorque(input->m_Backward);
	m_Wheel[2].m_M += m_Engine.getWheelM(2) - m_Wheel[2].getBrakeTorque(input->m_Backward);
	m_Wheel[3].m_M += m_Engine.getWheelM(3) - m_Wheel[3].getBrakeTorque(input->m_Backward);

	//adding the opposite torques to the body
	for(unsigned int i=0; i < 3; i++)
	{
		CVector wz = m_Wheel[i].m_Z;

		//make sure that the z axis is not flipped
		if(i == 1 || i == 3)
			wz = -wz;

		//x direction: (TODO: write out explicitly; this is inefficient)
		CVector wx = wz.crossProduct(CVector(0,-1,0));

		addTorque(-m_Wheel[i].m_M * wx);
	}
}

void CCar::calculateNormalForces()
{
	for(unsigned int i=0; i < 4; i++)
	{
		//y difference:
		float y = m_Wheel[i].m_Height, dy = m_Wheel[i].m_dHeight;
		//printf("Wheel %d: y = %.3f dy = %.3f\n", i, y, dy);

		m_Wheel[i].m_Fnormal = m_Wheel[i].m_suspk * y + m_Wheel[i].m_suspd * dy;
		if(m_Wheel[i].m_Fnormal < 0.0) m_Wheel[i].m_Fnormal = 0.0; //no "glue" forces
	}
}

void CCar::applyWheelForces()
{
	//This function mainly works in the car coordinate system
	CMatrix Rmat_inv = m_OrientationMatrix.transpose();

	CVector LinVel = Rmat_inv * m_Velocity;
	CVector AngVel = Rmat_inv * m_AngularVelocity;

	for(unsigned int i=0; i < 4; i++)
	{
		if(m_Wheel[i].m_Ground.nor.abs2() < 0.25 || m_Wheel[i].m_Ground.material == NULL)
			continue;

		float muGround = m_Wheel[i].m_Ground.material->m_Mu;
		float rollGround = m_Wheel[i].m_Ground.material->m_Roll;

		//position of the wheel:
		CVector r = m_Wheel[i].m_NeutralPos;

		//z axis:
		CVector wz = m_Wheel[i].m_Z;

		wz *= Rmat_inv;

		//make sure that the z axis is not flipped
		if(wz.z < 0.0)
			wz = -wz;

		//x direction:
		//TODO: write out explicitly (this is inefficient)
		CVector wx = wz.crossProduct(CVector(0,-1,0));

		//velocity of the center of the wheel:
		CVector Vcent = LinVel + r.crossProduct(AngVel);

		float vlong = Vcent.dotProduct(wz); //longitudinal velocity. Positive is backward
		float vlat =  Vcent.dotProduct(wx); //lateral velocity. Positive is right

		CVector Mwheel;
		CVector Fwheel = m_Wheel[i].getGroundForce(
			Mwheel, vlong, vlat, m_Wheel[i].m_Mu * muGround, m_Wheel[i].m_Roll * rollGround);

		//Transform from wheel coordinates to car coordinates:
		Fwheel = CVector(0,Fwheel.y,0) + Fwheel.x * wx + Fwheel.z * wz;

		//feedback to the net torque on the wheel:
		m_Wheel[i].m_M += Fwheel.z * m_Wheel[i].m_Radius + Mwheel.x;

		//forces attach on surface, not on middle:
		r += CVector(0.0, -m_Wheel[i].m_Radius, 0.0);

		//fprintf(stderr, "%f\t%f\t%f\t%f\t%f\t",
		//	wz.x, wz.y, wz.z,
		//	vlong, vlat);

		//Back to world orientation:
		Fwheel *= m_OrientationMatrix; //TODO: transform to ground plane
		r *= m_OrientationMatrix;

		//add forces to the wheel
		addForceAt(Fwheel, r);
	}

	//fprintf(stderr, "\n");
}

CBinBuffer &CCar::getData(CBinBuffer &b) const
{
	CMovingObject::getData(b);

	//general
	b.addFloat8(m_DesiredSteering, 0.008);
	//b.addFloat8(m_xAngle, 0.002);
	//b.addFloat8(m_zAngle, 0.002);

	//engine
	b += Uint8(m_Engine.m_Gear);
	b.addFloat16(m_Engine.m_MainAxisW, 0.4);
	b.addFloat8(m_Engine.m_Gas, 0.008);

	//wheels
	for(unsigned int i=0; i < 4; i++)
	{
		b.addFloat16(m_Wheel[i].m_w, 0.4);
		b.addFloat8(m_Wheel[i].m_a, 0.025);
		b.addFloat8(m_Wheel[i].m_DesiredSt, 0.008);
		b.addFloat8(m_Wheel[i].m_SkidVolume, 0.008);
		b.addFloat8(m_Wheel[i].m_Height, 0.008);
	}

	return b;
}

bool CCar::setData(const CBinBuffer &b, unsigned int &pos)
{
	if(!CMovingObject::setData(b, pos)) return false;

	//general
	m_DesiredSteering = b.getFloat8(pos, 0.008);
	//m_xAngle = b.getFloat8(pos, 0.002);
	//m_zAngle = b.getFloat8(pos, 0.002);

	//engine
	m_Engine.m_Gear = b.getUint8(pos);
	m_Engine.m_MainAxisW = b.getFloat16(pos, 0.4);
	m_Engine.m_Gas = b.getFloat8(pos, 0.008);

	//wheels
	for(unsigned int i=0; i < 4; i++)
	{
		m_Wheel[i].m_w = b.getFloat16(pos, 0.4);
		m_Wheel[i].m_a = b.getFloat8(pos, 0.025);
		m_Wheel[i].m_DesiredSt = b.getFloat8(pos, 0.008);
		m_Wheel[i].m_SkidVolume = b.getFloat8(pos, 0.008);
		m_Wheel[i].m_Height = b.getFloat8(pos, 0.008);
	}

	placeBodies();

	return true;
}
