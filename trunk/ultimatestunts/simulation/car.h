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

#include "carengine.h"
#include "carwheel.h"
#include "carrulestatus.h"

/**
  *@author CJP
  */

struct SCarDashboardInfo
{
	CString background_tex;
	float background_hth;

	CString steer_tex;
	CVector steer_pos;
	float steer_rad;
	float steer_ang;

	CString analog_vel_tex;
	CVector analog_vel_pos;
	float analog_vel_rad;
	float analog_vel_an0;
	float analog_vel_an1;
	float analog_vel_max;

	CString analog_rpm_tex;
	CVector analog_rpm_pos;
	float analog_rpm_rad;
	float analog_rpm_an0;
	float analog_rpm_an1;
	float analog_rpm_max;

	CVector digital_vel_pos;
	float digital_vel_hth;
	float digital_vel_wth;

	CVector digital_rpm_pos;
	float digital_rpm_hth;
	float digital_rpm_wth;

	CVector songtitle_pos;
	float songtitle_hth;
	float songtitle_wth;
};

class CCar : public CMovingObject  {
public: 
	CCar(CDataManager *manager);
	virtual ~CCar();

	virtual bool load(const CString &filename, const CParamList &list);
	virtual void unload();

	virtual void resetBodyPositions();

	virtual CBinBuffer &getData(CBinBuffer &b) const;            //override for car-specific data
	virtual bool setData(const CBinBuffer &b, unsigned int &pos);//override for car-specific data
	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::car;}

	CCarRuleStatus m_RuleStatus;

	CString m_CarName; //is loaded from car file

	virtual void update(CPhysics *simulator, float dt);
	virtual void correctCollisions();

	//sub objects:
	CCarEngine m_Engine;
	CCarWheel m_Wheel[4];

	//State variables:
	float m_DesiredSteering;
	float m_xAngle, m_zAngle; //orientation of the body
	float m_BodyHeight; //height of the body

	SCarDashboardInfo m_Dashboard;
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

	virtual void determineGroundPlane(CPhysics *simulator);
	void placeOnGround();
	void fixFlyingOrientation();

	virtual void placeBodies();

	//car specific settings:

	//body
	CVector m_BodySize;
	float m_cwA, m_RotationDamping;
	float m_PositionAboveGround;
	CVector m_CenterOfMass; //relative to body model

	//wheels
	float m_FrontSteerMax, m_RearSteerMax;
	float m_FrontDownforce, m_RearDownforce;
};

#endif
