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

#include "aiplayercar.h"
#include "carinput.h"

CAIPlayerCar::CAIPlayerCar() : CPlayer()
{
	m_FirstTime = true;
}

CAIPlayerCar::~CAIPlayerCar(){
}

bool CAIPlayerCar::update()
{

	if(m_MovingObjectId < 0) return false; //player not registered
	CMessage *input = theWorld->getMovingObject(m_MovingObjectId)->m_InputData;

	if(input->getType() != CMessageBuffer::carInput)
		{printf("Error: object input is not of type \"car\".\n"); return false;} //perhaps throw something

	CCarInput *carin = (CCarInput *)input;

	if(m_FirstTime)
	{
		printf("AI Player %d is setting input of object %d\n", m_PlayerId, m_MovingObjectId);
		
		m_FirstTime = false;
		return true; //input has changed
	}

	carin->m_Forward = 1.0;
	carin->m_Backward = 0.0;
	carin->m_Right = -0.1;
	carin->m_Gear = 1;
	carin->m_CarHorn = true;
	setAutomaticGear(carin->m_Forward, carin->m_Backward);

	return true; //input has changed
}
