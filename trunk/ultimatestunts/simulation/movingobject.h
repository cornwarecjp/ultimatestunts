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
#include "dynamicobject.h"
#include "vector.h"
#include "matrix.h"
#include "body.h"

class CPhysics;

class CMovingObject : public CDynamicObject
{
public:
	CMovingObject();
	~CMovingObject();

	//Various gets
	float getInvMass() const {return m_InvMass;}
	const CMatrix &getInvMomentInertia() const {return m_InvMomentInertia;}
	const CMatrix &getActualInvMomentInertia() const {return m_ActualInvMomentInertia;}

	CVector getPosition() const {return m_Position;}
	CVector getMomentum() const {return m_Momentum;}
	CVector getVelocity() const {return m_InvMass * m_Momentum;}

	CVector getOrientation() const {return m_Orientation;}
	const CMatrix &getOrientationMatrix() const {return m_OrientationMatrix;}
	CVector getAngularMomentum() const {return m_AngularMomentum;}
	CVector getAngularVelocity() const {return getActualInvMomentInertia() * m_AngularMomentum;}

	//Various sets
	void setPosition(CVector v){m_Position = v;}
	void setMomentum(CVector v){m_Momentum = v;}

	void setOrientation(CVector v);
	void setOrientationMatrix(const CMatrix &M);
	void setAngularMomentum(CVector v){m_AngularMomentum = v;}

	//Old position + orientation sub-API (for collision detection)
	void rememberCurrentState();
	virtual CVector getPreviousPosition() const {return m_PreviousPosition;}
	virtual const CMatrix &getPreviousOrientationMatrix() const {return m_PreviousOrientationMatrix;}

	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::movingObject;}

	//Update: physics simulation
	virtual void update(CPhysics *simulator, float dt);

	vector<CBody> m_Bodies; //The object bodies
	vector<int> m_Sounds; //The object sounds

	//For network transfer & other stuff
	virtual CBinBuffer &getData(CBinBuffer &b) const;       // returns class data as binbuffer
	virtual bool setData(const CBinBuffer &b);   // rebuild class data from binbuffer

protected:
	//constant:
	float m_InvMass;
	CMatrix m_InvMomentInertia;
	float cwA;

	//state variables:
	CVector m_Position, m_Momentum;
	CVector m_Orientation, m_AngularMomentum;

	//derived variables:
	CMatrix m_OrientationMatrix, m_ActualInvMomentInertia;

	//Remember for collision detection
	CVector m_PreviousPosition;
	CMatrix m_PreviousOrientationMatrix;

	virtual void getForces(CVector &Ftot, CVector &Mtot);
	void getAllForces(CPhysics *simulator, CVector &Ftot, CVector &Mtot); //including contact forces

	void updateActualInvMomentInertia();
};

#endif
