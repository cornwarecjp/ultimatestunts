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

	virtual CBinBuffer &getData(CBinBuffer &b) const;            //override for car-specific data
	virtual bool setData(const CBinBuffer &b, unsigned int &pos);//override for car-specific data
	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::car;}

	//CRuleStatus m_RuleStatus;

	virtual void update(CPhysics *simulator, float dt);

	//internal information
	float getGearRatio(int gear = -1);
	float getEngineTorque(float wengine);

	//State variables:
	unsigned int m_Gear;
	float m_MainAxisVelocity;
	float m_w1, m_w2, m_w3, m_w4; //angular velocities of the wheels
	float m_a1, m_a2, m_a3, m_a4; //rotation angles of the wheels
	float m_DesiredSteering;
	float m_gas;
	
protected:
	//car specific physics
	void simulateGeneral(CPhysics *simulator, float dt);
	void simulateAir(CPhysics *simulator, float dt);
	void simulateGround(CPhysics *simulator, float dt);

	void updateWheelOrientation();
	void updateMainAxisVelocity(float dt); //from the wheel velocities
	void updateMainAxisTorque();           //engine + gearbox simulation
	void updateWheelTorques();             //engine + brakes
	void applyWheelForces();               //tyre surface

	void addDownforce();         //aerodynamic downforce
	void doSteering(float dt);

	void placeOnGround(const CCollisionFace *theGround);

	virtual void placeBodies();

	//Temorary data
	CVector m_Zwheel1, m_Zwheel2, m_Zwheel3, m_Zwheel4; //z axis of the wheels
	float m_M1, m_M2, m_M3, m_M4; //engine+brake torques on the wheels
	float m_MainAxisTorque;

	//desired steering angles of the wheels
	float m_DesiredSt1, m_DesiredSt2, m_DesiredSt3, m_DesiredSt4;


	//car specific settings:

	//body
	float m_BodyMu;
	CVector m_BodySize;
	float m_cwA, m_RotationDamping;

	//wheels
	float m_FrontWheelMu, m_RearWheelMu;
	CVector m_FrontWheelNeutral;
	CVector m_RearWheelNeutral;
	float m_FrontWheelRadius, m_RearWheelRadius;
	float m_FrontWheelWidth, m_RearWheelWidth;
	float m_FrontBrakeMax, m_RearBrakeMax, m_FrontSteerMax, m_RearSteerMax;
	float m_FrontTraction, m_RearTraction;
	float m_FrontDownforce, m_RearDownforce;

	//driving pipeline
	float m_EngineTorque, m_MaxEngineSpeed, m_dMdw;
	vector<float> m_GearRatios;
	float m_DifferentialRatio;
};

#endif
