/***************************************************************************
                          humanplayer.cpp  -  A player-class getting input data from the real world ;-)
                             -------------------
    begin                : Thu Dec 5 2002
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

#include "humanplayer.h"

#include "carinput.h"

CHumanPlayer::CHumanPlayer(const CWorld *w) : CPlayer(w)
{
	m_KeysAreSet = false;
}

CHumanPlayer::~CHumanPlayer(){
}

bool CHumanPlayer::update()
{
	if(m_KeysAreSet && m_MovingObjectId >= 0)
	{
		CMessage *input = m_World->m_MovObjs[m_MovingObjectId]->m_InputData;

		if(input->getType() != CMessageBuffer::carInput)
			{printf("Error: object input is not of type \"car\".\n"); return false;} //perhaps throw something

		CCarInput *carin = (CCarInput *)input;

		carin->m_Forward = 1.0 * m_KeyState[SDLK_UP];
		carin->m_Backward = 1.0 * m_KeyState[SDLK_DOWN];
		carin->m_Right = 1.0 * m_KeyState[SDLK_RIGHT] - 1.0 * m_KeyState[SDLK_LEFT];
		carin->m_Gear = 1;
		carin->m_CarHorn = true;

		m_KeysAreSet = false;

		return true; //input has changed
	}

	return false;
}
