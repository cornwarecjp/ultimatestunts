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
#include <cstdio>

#include "movingobject.h"
#include "movobjinput.h"
#include "physics.h"

CMovingObject::CMovingObject(CDataManager *manager) : CDataObject(manager, CDataObject::eMovingObject)
{
	//default mass:
	m_InvMass = 1.0;

	m_InputData = new CMovObjInput;

	m_LastNetworkUpdateTime = -1000.0; //first update will always come through this

	m_Ground.nor = CVector(0,0,0);
}

CMovingObject::~CMovingObject()
{
	unload();
	delete m_InputData; //I guess this will happen for all CMovingObject-derived classes
}

bool CMovingObject::load(const CString &filename, const CParamList &list)
{
	CDataObject::load(filename, list);

	m_MovObjID = list.getValue("ID", "0").toInt();
	m_InputData->m_MovObjID = m_MovObjID;

	return true;
}

void CMovingObject::unload()
{
	//for(unsigned int i=0; i < m_Bodies.size(); i++)
	//	m_Bodies[i].destroyODE();
	m_Bodies.clear();

	m_Sounds.clear();
	m_Textures.clear();

	CDataObject::unload();
}

void CMovingObject::addForce(CVector F)
{
	m_Ftot += F;
}

void CMovingObject::addTorque(CVector M)
{
	m_Mtot += M;
}

void CMovingObject::addForceAt(CVector F, CVector pos)
{
	m_Ftot += F;
	m_Mtot += F.crossProduct(pos);

	/*
	CVector Madd = F.crossProduct(pos);
	printf("    F = %s\n", CString(F).c_str());
	printf("    p = %s\n", CString(pos).c_str());
	printf("    M = %s\n", CString(Madd).c_str());
	*/
}

void CMovingObject::update(CPhysics *simulator, float dt)
{
	//Integration step of the simulation:

	//printf("Ftot = (%s) kN\n", CString(m_Ftot/1000.0).c_str());
	
	//linear things:
	CVector accel = m_Ftot * m_InvMass;

	m_Position += (m_Velocity + 0.5*dt*accel)*dt;
	m_Velocity += dt*accel;

	//angular things:
	CVector angaccel = m_InvInertia * m_Mtot; //TODO: inertia tensor

	CMatrix dM;
	dM.setRotation((m_AngularVelocity + 0.5*dt*angaccel)*dt);
	m_OrientationMatrix *= dM;
	m_AngularVelocity += dt*angaccel;


	//Place the bodies:
	placeBodies();

	//Reset the forces:
	m_Ftot = CVector (0,0,0);
	m_Mtot = CVector (0,0,0);
}

void CMovingObject::determineGroundPlane(CPhysics *simulator)
{
	//TODO: some good default for non-car moving objects
}

void CMovingObject::correctCollisions()
{
	for(unsigned int c=0; c < m_SimCollisions.size(); c++)
	{
		CCollisionData col = m_SimCollisions[c];

		//printf("depth = %.3f\n", col.depth);
		CVector dr = col.nor * col.depth;

		//correct the position
		m_Position += dr;

		//set the collision velocity to zero
		float radcomp = m_Velocity.dotProduct(col.nor);
		if(radcomp < 0.0)
			m_Velocity -= radcomp * col.nor;
	}
}

CBinBuffer &CMovingObject::getData(CBinBuffer &b) const
{
	b += (Uint8)m_MovObjID;

	b.addFloat32(m_LastUpdateTime, 0.005); //more accurate than 1/100 sec

	CVector
		p = m_Position,
		o = m_OrientationMatrix.getRotation(),
		v = m_Velocity,
		w = m_AngularVelocity;
	
	b.addVector32(p, 0.001);
	b.addVector16(o, 0.0002);
	b.addVector16(v, 0.01);
	b.addVector16(w, 0.01);

	return b;
}

bool CMovingObject::setData(const CBinBuffer &b, unsigned int &pos)
{
	Uint8 ID = b.getUint8(pos);
	if(ID != m_MovObjID) return false; //wrong delivery, in some way

	float time = b.getFloat32(pos, 0.005);
	//printf("Received package: sendTime = %.2f\n", time);
	if(time < m_LastNetworkUpdateTime)
	{
		printf("Old package: time %.2f < %.2f\n", time, m_LastNetworkUpdateTime);
		return false; //it was an old package
	}
	if(time > theWorld->m_LastTime + 3.0)
	{
		printf("Too late package: time %.2f > %.2f + 3.0\n", time, theWorld->m_LastTime);
		return false; //probably a package from a previous game session
	}

	m_LastNetworkUpdateTime = time;
	m_LastUpdateTime = theWorld->m_LastTime;

	//TODO: correct game time for average lag time

		CVector
			p = b.getVector32(pos, 0.001),
			o = b.getVector16(pos, 0.0002),
			v = b.getVector16(pos, 0.01),
			w = b.getVector16(pos, 0.01);

	/*
	A little hack:
	We do not REALLY set the position to the position in the message.
	Instead, we mix it with the current position.
	This damps out "synchronisation errors"
	*/
	float dampFactor = v.abs2() / (10.0 + v.abs2()); //no damping at low speeds
	float vdif2 = (v - m_Velocity).abs2();
	dampFactor *= 10.0 / (10.0 + vdif2); //lower damping at higher differences in speed (e.g. collisions)
	if(dampFactor > 0.95) dampFactor = 0.95; //not too much damping
	//printf("%.3f\n", dampFactor);

	m_Position = (1.0 - dampFactor) * p + dampFactor * m_Position; //some damping to correct for "synchronisation noise"
	m_OrientationMatrix.setRotation(o);
	m_Velocity = v;
	m_AngularVelocity = w;

	return true;
}
