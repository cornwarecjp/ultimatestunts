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

		//Simulation stuff:
		virtual void simulate(CPhysics &theSimulator);

		//Position+veloctiy gets+sets
		virtual CVector getPosition(){return m_Position;}
		virtual void setPosition(CVector v){m_Position = v;}
		virtual CVector getVelocity(){return m_Velocity;}
		virtual void setVelocity(CVector v){m_Velocity = v;}
		virtual CVector getOrientationVector(){return m_OrientationVector;}
		virtual void setOrientationVector(CVector v);
		virtual CVector getAngularVelocity(){return m_AngularVelocity;}
		virtual void setAngularVelocity(CVector v){m_AngularVelocity = v;}

		virtual const CMatrix &getOrientation(){return m_Orientation;}
		virtual void setOrientation(const CMatrix &M);

		//Old position + orientation
		void rememberCurrentState();
		virtual CVector getPreviousPosition(){return m_PreviousPosition;}
		virtual const CMatrix &getPreviousOrientation(){return m_PreviousOrientation;}

		virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::movingObject;}

		virtual void updateBodyData() = 0;

		vector<CBody> m_Bodies; //The object bodies
		vector<int> m_Sounds; //The object sounds

		//constant:
		float m_InvMass;
		CMatrix m_InvMomentInertia;
	protected:
		CVector m_Position,
			m_PreviousPosition,
			m_Velocity,
			m_OrientationVector,
			m_AngularVelocity;
		CMatrix m_Orientation,
			m_PreviousOrientation;
};

#endif
