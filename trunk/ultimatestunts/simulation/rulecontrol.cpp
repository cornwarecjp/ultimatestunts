/***************************************************************************
                          rulecontrol.cpp  -  Check if players follow the rules
                             -------------------
    begin                : do sep 11 2003
    copyright            : (C) 2003 by CJP
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

#include <cstdio> //debugging

#include "rulecontrol.h"
#include "usmacros.h"

CRuleControl::CRuleControl()
{
	firstUpdate = true;
}

CRuleControl::~CRuleControl(){
}

bool CRuleControl::update()
{
	if(firstUpdate)
	{
		firstUpdate = false;
		theWorld->m_GameStartTime = m_Timer.getTime() + 3.0;

		if(!findStartFinish()) //error with starts/finishes
			return false; //stop

		placeStart();
		theWorld->m_Paused = true;
	}

	if(theWorld->m_Paused && m_Timer.getTime() > theWorld->m_GameStartTime)
		theWorld->m_Paused = false;

	return !checkFinished();
}

bool CRuleControl::findStartFinish()
{
	bool founds = false, foundf = false;

	int wth = theWorld->getTrack()->m_W;
	int hth = theWorld->getTrack()->m_H;
	for(int x = 0; x < theWorld->getTrack()->m_L; x++)
		for(int y = 0; y < wth; y++)
			for(int h = 0; h < hth; h++)
			{
				STile &tile = theWorld->getTrack()->m_Track[h + hth*(y + wth*x)];
				if(theWorld->getTileModel(tile.m_Model)->m_isStart)

					if(founds) //more than 1 start position
						{printf(">1 start\n"); return false;}
					else
					{
						founds = true;
						m_StartX = x; m_StartY = y; m_StartH = tile.m_Z;
					}

				if(theWorld->getTileModel(tile.m_Model)->m_isFinish)

					if(foundf) //more than 1 finish position
						{printf(">1 finish\n"); return false;}
					else
					{
						foundf = true;
						m_FinishX = x; m_FinishY = y; m_FinishH = tile.m_Z;
					}
			}

	if(!founds || !foundf) //no start or finish position
		return false;

	return true;
}

void CRuleControl::placeStart()
{
	CVector tilePos = CVector(
		TILESIZE * m_StartX,
		VERTSIZE * m_StartH,
		TILESIZE * m_StartY
	);

	/*
	Starting positions:
	*---*
	|0 1|
	|2 3|
	|4 5|
	|etc|
	*---*
	*/
	for(unsigned int i=0; i<theWorld->getNumObjects(CDataObject::eMovingObject); i++)
	{
		CVector carPos = CVector(
			-3.0*(i%2==0) +3.0*(i%2!=0),
			0.4,
			6.0*(i-i%2) + 3.0
		);

		theWorld->getMovingObject(i)->resetBodyPositions(tilePos + carPos, CMatrix());
	}
}

bool CRuleControl::checkFinished()
{
	return false; //not everybody's finished -> continue
}
