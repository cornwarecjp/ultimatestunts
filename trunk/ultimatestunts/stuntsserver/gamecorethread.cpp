/***************************************************************************
                          gamecorethread.cpp  -  The thread of the simulation
                             -------------------
    begin                : wo jan 12 2005
    copyright            : (C) 2005 by CJP
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

#include "gamecorethread.h"

#include <unistd.h>
#include <cstdio>

CGamecoreThread::CGamecoreThread(){
}
CGamecoreThread::~CGamecoreThread(){
}

void CGamecoreThread::threadFunc()
{
	//extremely simple:
	//keep running until the game has ended
	//or a stop command is given
	while( m_GameCore->update() && (!m_terminate) )
		usleep(10);
}
