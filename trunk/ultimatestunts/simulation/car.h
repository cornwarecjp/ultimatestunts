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
	CCar(CDataManager *manager);
	virtual ~CCar();

	virtual bool load(const CString &filename, const CParamList &list);
	virtual void unload();

	virtual void resetBodyPositions(CVector pos, const CMatrix &ori);

	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::car;}

	//CRuleStatus m_RuleStatus;

	virtual void update(CPhysics *simulator, float dt);

	//internal information
	float getGearRatio(int gear = -1);
	float getEngineTorque(float wengine);

	//Contact info
	dJointID m_joint1, m_joint2, m_joint3, m_joint4;

	//State variables:
	unsigned int m_Gear;
	float m_MainAxisVelocity;
	float m_MainAxisTorque;
	float m_DesiredSteering;
	float m_gas;
	
protected:
	//car specific physics
	void updateAxisData();
	void updateMainAxisVelocity(float dt);  //from the wheel velocities
	void updateMainAxisTorque(); //engine + gearbox simulation
	void applyWheelTorques();    //engine + brakes
	void addDownforce();         //aerodynamic downforce
	void doSteering(float dt);

	//Cached data about the wheels' axes
	CVector m_a1, m_a2, m_a3, m_a4;

	//desired steering angles of the wheels
	float m_DesiredSt1, m_DesiredSt2, m_DesiredSt3, m_DesiredSt4;


	//car specific settings:

	//body
	float m_BodyMu, m_BodyMass;
	CVector m_BodySize;
	float m_cwA, m_RotationDamping;

	//wheels
	float m_FrontWheelMu, m_RearWheelMu;
	CVector m_FrontWheelNeutral;
	CVector m_RearWheelNeutral;
	float m_FrontWheelRadius, m_RearWheelRadius;
	float m_FrontWheelWidth, m_RearWheelWidth;
	float m_FrontWheelMass, m_RearWheelMass;
	float m_FrontStopERP, m_RearStopERP, m_FrontStopCFM, m_RearStopCFM;
	float m_FrontSuspERP, m_RearSuspERP, m_FrontSuspCFM, m_RearSuspCFM;
	float m_FrontBrakeMax, m_RearBrakeMax, m_FrontSteerMax, m_RearSteerMax;
	float m_FrontTraction, m_RearTraction;
	float m_FrontDownforce, m_RearDownforce;

	//driving pipeline
	float m_EngineTorque, m_MaxEngineSpeed, m_dMdw;
	vector<float> m_GearRatios;
	float m_DifferentialRatio;
};

#endif
