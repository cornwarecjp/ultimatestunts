/***************************************************************************
                          car.h  -  A car, being a moving object
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

#ifndef CAR_H
#define CAR_H

#include "movingobject.h"

/**
  *@author CJP
  */

class CCar : public CMovingObject  {
public: 
	CCar();
	~CCar();

	virtual void resetBodyPositions(CVector pos, const CMatrix &ori);

	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::car;}

	//CRuleStatus m_RuleStatus;

	virtual void update(CPhysics *simulator, float dt);

	//The collision info for ODE
	dGeomID m_body, m_wheel1, m_wheel2, m_wheel3, m_wheel4;
	dGeomID m_geomgroup;

	//Contact info
	dJointID m_joint1, m_joint2, m_joint3, m_joint4;
	
	//constant
	CVector m_FrontWheelNeutral;
	CVector m_BackWheelNeutral;
	float m_WheelRadius;
	float m_cwA;
};

#endif
