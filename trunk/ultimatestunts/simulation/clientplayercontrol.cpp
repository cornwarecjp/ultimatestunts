/***************************************************************************
                          clientplayercontrol.cpp  -  Remote player management
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

#include "clientplayercontrol.h"

CClientPlayerControl::CClientPlayerControl(CClientNet *net) : CPlayerControl()
{
	m_Net = net;
}

CClientPlayerControl::~CClientPlayerControl()
{
}

int CClientPlayerControl::addPlayer(CObjectChoice choice)
{
	return CPlayerControl::addPlayer(choice);
}

bool CClientPlayerControl::loadObjects()
{
	return CPlayerControl::loadObjects();
}
