/***************************************************************************
                          player.cpp  -  Base-class for players
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

#include "player.h"
#include "car.h"
#include "carinput.h"

CPlayer::CPlayer()
{
	m_World = theWorld;

  //These have not been set:
  m_MovingObjectId = -1;
  m_PlayerId = -1;
}

CPlayer::~CPlayer()
{
}

int CPlayer::setAutomaticGear(float &gas, float &brake)
{
	if(m_MovingObjectId < 0)
		return -1; //not set because not connected to a moving object

	if(theWorld->getMovingObject(m_MovingObjectId)->getType() != CMessageBuffer::car)
		return -2;

	CCar *theCar = (CCar *)theWorld->getMovingObject(m_MovingObjectId);
	CCarInput *carin = (CCarInput *)theCar->m_InputData;

	int gear = theCar->m_Engine.m_Gear;
	int maxgear = 6;
	float r = theCar->m_Engine.getGearRatio();
	float w = theCar->m_Engine.m_MainAxisW * r;
	float M = theCar->m_Engine.getEngineM(w) * r;

	float speed = theCar->m_Velocity.abs();

	if(gear < maxgear && gear > 0) //gear up if possible (and no reverse gear)
	{
		float rhigher = theCar->m_Engine.getGearRatio(gear+1);
		float whigher = theCar->m_Engine.m_MainAxisW * rhigher;
		float Mhigher = theCar->m_Engine.getEngineM(whigher) * rhigher;

		if(Mhigher > M)
		{
			carin->m_Gear = gear + 1;
			return carin->m_Gear;
		}
	}

	if(gear > 1) //gear down if possible
	{
		float rlower = theCar->m_Engine.getGearRatio(gear-1);
		float wlower = theCar->m_Engine.m_MainAxisW * rlower;
		float Mlower = theCar->m_Engine.getEngineM(wlower) * rlower;
		
		if(Mlower > M)
		{
			carin->m_Gear = gear - 1;
			return carin->m_Gear;
		}
	}

	if(gear == 1 && brake > gas && speed < 1.0) //go to reverse gear
	{
		float t = gas;
		gas = brake;
		brake = t;
		carin->m_Gear = 0;
		return carin->m_Gear;
	}

	if(gear == 0)
	{
		if(gas > brake && speed < 1.0) //leave reverse gear
		{
			carin->m_Gear = 1;
			return carin->m_Gear;
		}

		float t = gas;
		gas = brake;
		brake = t;
	}

	carin->m_Gear = gear;
	return gear;
}
