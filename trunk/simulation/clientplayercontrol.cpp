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

#include <cstdio>
#include "clientplayercontrol.h"
#include "netmessages.h"

CClientPlayerControl::CClientPlayerControl(CClientNet *net) : CPlayerControl()
{
	m_Net = net;
}

CClientPlayerControl::~CClientPlayerControl()
{
}

int CClientPlayerControl::addPlayer(CObjectChoice choice)
{
	CMessageBuffer buf = choice.getBuffer();
	int ret = m_Net->sendDataReliable(buf);
	if(ret < 0 || ret == OBJECTCHOICE_REFUSED)
	{
		return -1;
	}

	return ret;
}

bool CClientPlayerControl::loadObjects()
{
	clearPlayerList();

	//TODO: find a way to make this reliable
	m_Net->sendTextMessage(USNET_OBJECTS);
	while(true)
	{
		CMessageBuffer *b = m_Net->receiveExpectedData(CMessageBuffer::objectChoice, 1000);
		if(b == NULL) break;

		if(b->getAC() != 0) //server wants confirmation
			m_Net->sendConfirmation(*b, 0);

		CObjectChoice oc;
		oc.setBuffer(*b);
		delete b;

		if(oc.m_ObjType == CMessageBuffer::badMessage) break;

		m_LocalChoices.push_back(oc);
	}

	return CPlayerControl::loadObjects();
}
