/***************************************************************************
                          aiplayercar.cpp  -  description
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

#include <stdio.h>

#include "aiplayercar.h"
#include "carinput.h"

CAIPlayerCar::CAIPlayerCar(CWorld *w) : CPlayer(w)
{
	m_FirstTime = true;
}

CAIPlayerCar::~CAIPlayerCar(){
}

bool CAIPlayerCar::Update()
{
	if(m_MovingObjectId < 0) return false; //player not registered

	CMessage *input = m_World->m_MovObjs[m_MovingObjectId]->m_InputData;

	if(input->getType() != CMessageBuffer::carInput)
		{printf("Error: object input is not of type \"car\".\n"); return false;} //perhaps throw something

	CCarInput *carin = (CCarInput *)input;

	if(m_FirstTime)
	{
		printf("AI Player %d is setting input of object %d\n", m_PlayerId, m_MovingObjectId);
		carin->m_Forward = 1.0;
		carin->m_Backward = 0.0;
		carin->m_Right = -0.5;
		carin->m_Gear = 1;
		carin->m_CarHorn = true;

		m_FirstTime = false;
		return true; //input has changed
	}

	return false; //input has not changed
}
