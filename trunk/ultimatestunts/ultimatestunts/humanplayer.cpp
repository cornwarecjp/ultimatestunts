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

CHumanPlayer::CHumanPlayer(CGameWinSystem *ws, unsigned int ID) : CPlayer()
{
	m_WinSys = ws;
	m_LocalHumanPlayerID = ID;
	m_AutomaticGear = true;
}

CHumanPlayer::~CHumanPlayer(){
}

bool CHumanPlayer::update()
{
	if(m_MovingObjectId >= 0)
	{
		float up = m_WinSys->getPlayerControlState(eUp, m_LocalHumanPlayerID);
		float down = m_WinSys->getPlayerControlState(eDown, m_LocalHumanPlayerID);
		float left = m_WinSys->getPlayerControlState(eLeft, m_LocalHumanPlayerID);
		float right = m_WinSys->getPlayerControlState(eRight, m_LocalHumanPlayerID);

		//This is called 'input', as it is the input of the moving object.
		//But it is the output of the player.
		CMessage *input = theWorld->getMovingObject(m_MovingObjectId)->m_InputData;

		//TODO: find some way to check what kind of input it is
		CCarInput *carin = (CCarInput *)input;

		carin->m_Forward = up;
		carin->m_Backward = down;
		carin->m_Right = right - left;

		//default
		carin->m_Gear = ((CCar *)theWorld->getMovingObject(m_MovingObjectId))->m_Engine.m_Gear;
		
		//try the automatic gear, if requested
		if(m_AutomaticGear)
			setAutomaticGear(carin->m_Forward, carin->m_Backward);

		//Try the manual thing
		if(m_WinSys->playerKeyWasPressed(eShiftUp, m_LocalHumanPlayerID) && carin->m_Gear < 6)
			carin->m_Gear++;
		if(m_WinSys->playerKeyWasPressed(eShiftDown, m_LocalHumanPlayerID) && carin->m_Gear > 0)
			carin->m_Gear--;
		
		carin->m_CarHorn = m_WinSys->playerKeyWasPressed(eHorn, m_LocalHumanPlayerID);

		return true; //input has changed (TODO: make a better system)
	}

	return false;
}
