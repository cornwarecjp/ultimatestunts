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
#include "collisionface.h"

#include "carengine.h"
#include "carwheel.h"
#include "carrulestatus.h"

/**
  *@author CJP
  */

class CCar : public CMovingObject  {
public: 
	CCar(CDataManager *manager);
	virtual ~CCar();

	virtual bool load(const CString &filename, const CParamList &list);
	virtual void unload();

	virtual void resetBodyPositions(CVector pos, const CMatrix &ori);

	virtual CBinBuffer &getData(CBinBuffer &b) const;            //override for car-specific data
	virtual bool setData(const CBinBuffer &b, unsigned int &pos);//override for car-specific data
	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::car;}

	CCarRuleStatus m_RuleStatus;

	virtual void update(CPhysics *simulator, float dt);
	virtual void correctCollisions();

	//sub objects:
	CCarEngine m_Engine;
	CCarWheel m_Wheel[4];

	//State variables:
	float m_DesiredSteering;
	float m_xAngle, m_zAngle; //orientation of the body

protected:
	//car specific physics
	void simulateGeneral(CPhysics *simulator, float dt);
	void simulateAir(CPhysics *simulator, float dt);
	void simulateGround(CPhysics *simulator, float dt);

	void updateWheelOrientation();
	void updateWheelTorques();             //engine + brakes
	void calculateNormalForces();          //vertical force through wheels
	void applyWheelForces();               //tyre surface

	void addDownforce();         //aerodynamic downforce
	void doSteering(float dt);

	CCollisionFace m_Ground; //the ground plane ( |normal| < 0.5 is no plane)
	void determineGroundPlane(CPhysics *simulator);
	void placeOnGround();

	virtual void placeBodies();

	//car specific settings:

	//body
	CVector m_BodySize;
	float m_cwA, m_RotationDamping;
	float m_PositionAboveGround;

	//wheels
	float m_FrontBrakeMax, m_RearBrakeMax, m_FrontSteerMax, m_RearSteerMax;
	float m_FrontDownforce, m_RearDownforce;
};

#endif
