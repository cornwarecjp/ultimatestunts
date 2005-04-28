/***************************************************************************
                          movingobject.h  -  Moving object class
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

#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H


/**
  *@author CJP
  */
#include "message.h"
#include "movobjinput.h"
#include "dataobject.h"

#include "collisiondata.h"

#include "vector.h"
#include "matrix.h"
#include "body.h"

class CPhysics;

class CMovingObject : public CDataObject, CMessage
{
public:
	CMovingObject(CDataManager *manager);
	~CMovingObject();

	virtual bool load(const CString &filename, const CParamList &list);
	virtual void unload();
	
	//physics simulation
	virtual void update(CPhysics *simulator, float dt);
	//collision response
	virtual void correctCollisions();

	//For network transfer & other stuff
	virtual CBinBuffer &getData(CBinBuffer &b) const;             //puts body positions etc. into buffer
	virtual bool setData(const CBinBuffer &b, unsigned int &pos); //rebuild class data from binbuffer
	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::movingObject;}

	//CMessage wrappers:
	CMessageBuffer getBuffer(){return CMessage::getBuffer();}
	bool setBuffer(const CMessageBuffer &b){return CMessage::setBuffer(b);}

	unsigned int getMovObjID()
		{return m_MovObjID;}

	virtual void resetBodyPositions(CVector pos, const CMatrix &ori)=0; //should be called after setting object position, orientation
	const CVector &getCameraPos() const
		{return m_CameraPos;}

	//static data:
	vector<unsigned int> m_Sounds; //The object sounds
	vector<unsigned int> m_Textures; //The object textures

	//dynamic data:
	vector<CBody> m_Bodies; //The object bodies
	CMovObjInput *m_InputData;

	//This one is used in correctCollisions(). Only the collisions of one simulation step.
	vector<CCollisionData> m_SimCollisions;
	//This is used in e.g. the sound subsystem
	vector<CCollisionData> m_AllCollisions;


	//State variables:
	CVector m_Velocity, m_AngularVelocity;
	CVector m_Position;
	CMatrix m_OrientationMatrix;

protected:
	CVector m_CameraPos; //the relative position of the inside camera

	Uint8 m_MovObjID;

	//Force accumulation:
	void addForce(CVector F);
	void addTorque(CVector M);
	void addForceAt(CVector F, CVector pos);

	//placing the bodies:
	virtual void placeBodies() = 0;

	//static data:
	float m_InvMass;
	CMatrix m_InvInertia;

	CVector m_Ftot, m_Mtot;
};

#endif
