/***************************************************************************
                          clientsim.cpp  -  Client-side networked simulation
                             -------------------
    begin                : di jan 14 2003
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

#include "clientsim.h"
#include "textmessage.h"
#include "movingobject.h"

#include "netmessages.h"

CClientSim::CClientSim(CGameCore *gc, CClientNet *net)
{
	m_GameCore = gc;
	m_Net = net;
	
	m_PreviousTime = theWorld->m_LastTime;
}

CClientSim::~CClientSim()
{
}

CString CClientSim::getTrackname(CGameCore::LoadStatusCallback callBackFun)
{
	m_Net->sendTextMessage(USNET_READY);

	while(true)
	{
		m_Net->sendTextMessage(USNET_STATUS); //request for status

		CMessageBuffer *buf = m_Net->receiveExpectedData(CMessageBuffer::textMessage, 500); //0.5 second
		if(buf != NULL)
		{
			CTextMessage tm;
			tm.setBuffer(*buf);

			if(buf->getAC() != 0)
				m_Net->sendConfirmation(*buf, 0);

			delete buf;

			//We received the track file data:
			CString trk = USNET_TRACK;
			if(tm.m_Message.mid(0,trk.length()) == trk)
				return tm.m_Message.mid(trk.length());

			//We received a status update:
			CString sts = USNET_STATUS;
			if(callBackFun != NULL && tm.m_Message.mid(0,sts.length()) == sts)
				callBackFun(tm.m_Message.mid(sts.length()), -1.0);
		}
	}

	return "tracks/default.track"; //we'll never reach this anyway
}

bool CClientSim::update()
{
	//the moving objects:
	vector<CDataObject *> objs = theWorld->getObjectArray(CDataObject::eMovingObject);

	float now = theWorld->m_LastTime;

	if(now < m_PreviousTime - 0.01) //then we obviously started a new game
		m_PreviousTime = now;

	//if some amount of time has passed:
	if(now > m_PreviousTime + 0.04) //25 times per second
	{
		m_PreviousTime = now;
		
		//send input info
		for(unsigned int j=0; j < objs.size(); j++)
		{
			CMovingObject *mo = (CMovingObject *)objs[j];

			if( !m_GameCore->isLocalPlayer(mo->getMovObjID()) ) continue; //only local players

			CMessageBuffer b = mo->m_InputData->getBuffer();
			m_Net->sendData(b);
		}
	}

	//receive object info
	//the receiveData function was already called by the gamecore
	for(unsigned int i=0; i < m_Net->m_ReceiveBuffer.size(); i++)
	{
		CMessageBuffer &buf = m_Net->m_ReceiveBuffer[i];

		//Tell the server that we received it (in cases when needed)
		if(buf.getAC() != 0)
			m_Net->sendConfirmation(buf, 0);

		switch(buf.getType())
		{
		case CMessageBuffer::car:
		case CMessageBuffer::movingObject:
		{
			CBinBuffer data = buf.getData();
			unsigned int pos = 0;
			Uint8 ID = data.getUint8(pos);

			//find the corresponding moving object
			CMovingObject *mo = NULL;
			//first candidate
			if(ID < objs.size())
			{
				CMovingObject *tmp = (CMovingObject *)objs[ID];
				if(tmp->getMovObjID() == ID)
					mo = tmp;
			}
			//all other objects
			if(mo == NULL)
				for(unsigned int j=0; j < objs.size(); j++)
				{
					CMovingObject *tmp = (CMovingObject *)objs[j];
					if(tmp->getMovObjID() == ID)
						mo = tmp;
				}

			if(mo == NULL) break; //not found

			mo->setBuffer(buf);
		}
			break;

		case CMessageBuffer::hiscore:
			m_Hiscore.setBuffer(buf);
			return false; //ending the game: we received a hiscore
			break;

		case CMessageBuffer::chat:
		{
			CChatMessage msg;
			msg.setBuffer(buf);
			theWorld->m_ChatSystem.m_InQueue.push_back(msg); //chat message received from server
			//printf("Chat message received: %s\n", msg.m_Message.c_str());
		}
			break;

		default:
			break;
		}

		//TODO: maybe delete the used messages (maybe not necessary)
	}

	return true;
}
