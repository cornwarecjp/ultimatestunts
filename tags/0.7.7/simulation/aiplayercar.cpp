/***************************************************************************
                          aiplayercar.cpp  -  An AI player for controlling cars
                             -------------------
    begin                : wo dec 18 2002
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

#include <cstdio>
#include <cmath>

#include "aiplayercar.h"
#include "world.h"

CAIPlayerCar::CAIPlayerCar() : CPlayer()
{
	m_FirstTime = true;
}

CAIPlayerCar::~CAIPlayerCar(){
}

bool CAIPlayerCar::update()
{

	if(m_MovingObjectId < 0) return false; //player not registered
	
	CMovingObject *object = theWorld->getMovingObject(m_MovingObjectId);
	CMovObjInput *input = object->m_InputData;

	//For now, assume that it's a car
	m_Car = (CCar *)object;
	m_Carin = (CCarInput *)input;

	if(m_FirstTime)
	{
		printf("AI Player %d is setting input of object %d\n", m_PlayerId, m_MovingObjectId);
		
		m_FirstTime = false;
		return true; //input has changed
	}

	m_Carin->m_CarHorn = true;

	//Do steering:
	setTargetOrientation(findNextTargetPos(false) - m_Car->m_Position);

	//Determine speed based on distance:
	CVector tgtpos = findNextTargetPos(true);
	float dist = (m_Car->m_Position - tgtpos).abs();
	float tgtvel = 0.3*dist + 25.0; //faster when further away

	//Limit speed by steering:
	float maxspeed = 12.5 / (fabsf(m_Carin->m_Right) + 0.0001);
	if(tgtvel > maxspeed) tgtvel = maxspeed;

	setTargetSpeed(tgtvel, m_Carin->m_Forward, m_Carin->m_Backward);

	m_Carin->m_Gear = 1;
	setAutomaticGear(m_Carin->m_Forward, m_Carin->m_Backward);

	return true; //input has changed
}

void CAIPlayerCar::setTargetSpeed(float v, float &gas, float &brake)
{
	float vnow = m_Car->m_Velocity.abs();
	float vdiff = v - vnow;

	gas = brake = 0.0;

	if(vdiff > 0.0)
	{
		gas = vdiff;
		if(gas > 1.0) gas = 1.0;
	}
	else
	{
		brake = -0.1*vdiff;
		if(brake > 1.0) brake = 1.0;
	}
}

CVector CAIPlayerCar::findNextTargetPos(bool far)
{
	CVector posNow = m_Car->m_Position;
	CCarRuleStatus &rules = m_Car->m_RuleStatus;
	const CTrack *track = theWorld->getTrack();

	//When the finish tile has been entered:
	if(rules.lastPosition[0] == int(track->m_Routes[0].size())-1)
	{
		const CTrack::CCheckpoint &check = track->m_Routes[0][rules.lastPosition[0]];
		return CVector(TILESIZE*check.x, VERTSIZE*check.y, TILESIZE*check.z);
	}

	unsigned int FollowedRoute = 0; //A constant for now

	//Current pos on route
	const CTrack::CCheckpoint &check = 
			track->m_Routes[FollowedRoute][rules.lastPosition[FollowedRoute]];
	CVector checkpos(TILESIZE*check.x, VERTSIZE*check.y, TILESIZE*check.z);

	//Direction on route
	const CTrack::CCheckpoint &nextcheck = 
		track->m_Routes[FollowedRoute][rules.lastPosition[FollowedRoute]+1];
	int dirx = nextcheck.x - check.x, dirz = nextcheck.z - check.z;

	//printf("%d,%d,%d; dir %d,%d\n", check.x, check.y, check.z, dirx, dirz);

	//Initial target: next tile on route
	CVector tgt = CVector(
			TILESIZE*nextcheck.x, VERTSIZE*nextcheck.y, TILESIZE*nextcheck.z
			);
	float dist2 = (tgt - posNow).abs2();

	//Find last check in same direction
	for(
		unsigned int j=rules.lastPosition[FollowedRoute]+1;
		j < track->m_Routes[FollowedRoute].size();
		j++)
	{
		const CTrack::CCheckpoint &thischeck = track->m_Routes[FollowedRoute][j];
		int dx = thischeck.x - check.x;
		int dz = thischeck.z - check.z;

		if(dx*dirz == dz*dirx) //same direction
		{
			CVector lastCheckPos(
				TILESIZE*thischeck.x, VERTSIZE*thischeck.y, TILESIZE*thischeck.z
				);

			if(far)
			{
				//Exception for finish (go full gas through finish)
				//set a target far behind the finish
				if(FollowedRoute == 0 && j == track->m_Routes[FollowedRoute].size()-1)
				{
					lastCheckPos.x += 10*TILESIZE*dirx;
					lastCheckPos.z += 10*TILESIZE*dirz;
				}

				tgt = lastCheckPos;
			}
			else
			{
				//In some situations, use a further tile as target
				float thisdist2 = (lastCheckPos - posNow).abs2();
				if(thisdist2 < dist2 || thisdist2 < TILESIZE)
					{dist2 = thisdist2; tgt = lastCheckPos;}
			}

		}
		else
			{break;}
	}

	return tgt;
}

void CAIPlayerCar::setTargetOrientation(CVector nosedir)
{
	CVector noseNow = m_Car->m_OrientationMatrix * CVector(0,0,-1);

	nosedir.y = noseNow.y = 0.0; //ignore pitch
	nosedir.normalise();
	noseNow.normalise();
	float sinus = nosedir.crossProduct(noseNow).y;

	if(nosedir.dotProduct(noseNow) < 0.0)
	{
		m_Carin->m_Right = copysignf(1.0, sinus);
	}
	else
	{
		m_Carin->m_Right = sinus;
	}

	//Choose a direction when the nose is almost in opposite direction
	if(nosedir.dotProduct(noseNow) < -0.9)
	{
		//printf("Backwards, turning forward\n");
		m_Carin->m_Right = 1.0;
	}
}
