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

#include <cstdio>

#include "humanplayer.h"

#include "car.h"
#include "carinput.h"

CHumanPlayer::CHumanPlayer(CWinSystem *ws) : CPlayer()
{
	m_WinSys = ws;
}

CHumanPlayer::~CHumanPlayer(){
}

bool CHumanPlayer::update()
{
	if(m_MovingObjectId >= 0)
	{
		const Uint8 *keystate = m_WinSys->getKeyState();
		CJoyState joy = m_WinSys->getJoyState(0);

		//This is called 'input', as it is the input of the moving object.
		//But it is the output of the player.
		CMessage *input = theWorld->m_MovObjs[m_MovingObjectId]->m_InputData;

		if(input->getType() != CMessageBuffer::carInput)
			{printf("Error: object input is not of type \"car\".\n"); return false;} //perhaps throw something

		CCarInput *carin = (CCarInput *)input;

		carin->m_Forward = 1.0 * keystate[SDLK_UP];
		carin->m_Backward = 1.0 * keystate[SDLK_DOWN];
		carin->m_Right = 1.0 * keystate[SDLK_RIGHT] - 1.0 * keystate[SDLK_LEFT];

		//Override with joystick:
#define THR 1000
		if(joy.x < -THR || joy.x > THR)
			carin->m_Right = (float)joy.x / 32767;
		if(joy.y > THR)
			carin->m_Backward = (float)joy.y / 32767;
		if(joy.y < -THR)
			carin->m_Forward = (float)-joy.y / 32767;

		//default
		carin->m_Gear = ((CCar *)theWorld->m_MovObjs[m_MovingObjectId])->m_Gear;
		
		//try the automatic gear
		setAutomaticGear(carin->m_Forward, carin->m_Backward);

		//Try the manual thing
		if(m_WinSys->wasPressed('a') && carin->m_Gear < 6)
			carin->m_Gear++;
		if(m_WinSys->wasPressed('z') && carin->m_Gear > 0)
			carin->m_Gear--;
		
		carin->m_CarHorn = true;

		return true; //input has changed (TODO: make a better system)
	}

	return false;
}
