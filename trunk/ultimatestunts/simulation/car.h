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

	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::car;}

	//CRuleStatus m_RuleStatus;

	virtual void updateBodyData();

	virtual void simulate(CPhysics &theSimulator);

	//constant
	CVector m_FrontWheelNeutral;
	CVector m_BackWheelNeutral;
	float m_WheelRadius;

	//Changing during the game
	float m_WheelVelocity, m_WheelAngle;
	float m_wheelHeight1, m_wheelHeight2, m_wheelHeight3, m_wheelHeight4;
};

#endif
