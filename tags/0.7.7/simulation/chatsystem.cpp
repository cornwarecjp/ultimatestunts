/***************************************************************************
                          chatsystem.cpp  -  The "mail delivery office" of Ultimate Stunts
                             -------------------
    begin                : do nov 24 2005
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

#include <cstdio>

#include "world.h"
 
#include "chatsystem.h"

CChatSystem::CChatSystem(){
}

CChatSystem::~CChatSystem(){
}

void CChatSystem::sendMessage(CChatMessage &msg)
{
	m_OutQueue.push_back(msg);
}

/*
vector<CChatMessage> CChatSystem::receiveMessages(int receiver) //-1 = all
{
	if(receiver < 0)
	{
		vector<CChatMessage> ret = m_InQueue;
		m_InQueue.clear();
		return ret;
	}

	vector<CChatMessage> ret;
	vector<CChatMessage> other;

	for(unsigned int i=0; i < m_InQueue.size(); i++)
		if(m_InQueue[i].m_ToMovingObject < 0) //sent to all players
		{
			ret.push_back(m_InQueue[i]);
			other.push_back(m_InQueue[i]);
		}
		else if(m_InQueue[i].m_ToMovingObject == receiver) //sent to this player
		{
			ret.push_back(m_InQueue[i]);
		}
		else //sent to other players
		{
			other.push_back(m_InQueue[i]);
		}

	m_InQueue = other;
	return ret;
}
*/

void CChatSystem::loopBack()
{
	m_InQueue = m_OutQueue;
	m_OutQueue.clear();
}

void CChatSystem::deliverMessages()
{
	for(unsigned int i=0; i < m_InQueue.size(); i++)
	{
		CChatMessage &msg = m_InQueue[i];

		for(unsigned int j=0; j < theWorld->getNumObjects(CDataObject::eMovingObject); j++)
		{
			CMovingObject *mo = theWorld->getMovingObject(j);
			if(msg.m_ToMovingObject < 0 || (unsigned int)(msg.m_ToMovingObject) == mo->getMovObjID())
			{
				mo->m_IncomingMessages.push_back(msg);
				//printf("Delivered message \"%s\" time %.2f\n", msg.m_Message.c_str(), msg.m_SendTime);
			}
		}
	}

	m_InQueue.clear();
}
