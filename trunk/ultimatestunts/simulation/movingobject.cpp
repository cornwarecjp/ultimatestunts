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
}

CMovingObject::~CMovingObject()
{
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

void CMovingObject::correctCollisions(const vector<CCollisionData> &cols)
{
	for(unsigned int c=0; c < cols.size(); c++)
	{
		CCollisionData col = cols[c];

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

	//TODO: update this

	for(unsigned int i=0; i < m_Bodies.size(); i++)
	{
		CVector
			p = m_Bodies[i].m_Position,
			o = m_Bodies[i].m_OrientationMatrix.getRotation();

		b.addVector32(p, 0.001);
		b.addVector16(o, 0.0002);
	}

	return b;
}

bool CMovingObject::setData(const CBinBuffer &b, unsigned int &pos)
{
	Uint8 ID = b.getUint8(pos);
	if(ID != m_MovObjID) return false;

	//TODO: update this

	//TODO: check the number of objects
	for(unsigned int i=0; i < m_Bodies.size(); i++)
	{
		CVector
			p = b.getVector32(pos, 0.001),
			o = b.getVector16(pos, 0.0002);

		m_Bodies[i].m_Position = p;
		m_Bodies[i].m_OrientationMatrix.setRotation(o);
	}

	return true;
}
