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

#include "rulecontrol.h"
#include "usmacros.h"

CRuleControl::CRuleControl(CWorld *w) : CSimulation(w)
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
		placeStart();
	}

	return !checkFinished();
}

void CRuleControl::placeStart()
{
	CVector tilePos = CVector(
		TILESIZE * (int)(0.6 * m_World->m_L),
		0.0,
		TILESIZE * (int)(0.6 * m_World->m_W)
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
	for(unsigned int i=0; i<m_World->m_MovObjs.size(); i++)
	{
		CVector carPos = CVector(
			-3.0*(i%2==0) +3.0*(i%2!=0),
			0.0,
			6.0*(i-i%2)
		);

		m_World->m_MovObjs[i]->setPosition(
			m_World->m_MovObjs[i]->getPosition() +
			tilePos +
			carPos
		);
	}
}

bool CRuleControl::checkFinished()
{
	return false; //not everybody's finished -> continue
}
