/***************************************************************************
                          networkthread.cpp  -  Listening to messages from clients
                             -------------------
    begin                : do jan 13 2005
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

#include <unistd.h>

#include "networkthread.h"
#include "chatmessage.h"
#include "textmessage.h"

CNetworkThread::CNetworkThread(unsigned int port)
{
	m_Clients = NULL;
	m_Port = port;
}

CNetworkThread::~CNetworkThread()
{
	stop();
}

void CNetworkThread::giveClientList(CClientList *clients)
{
	m_Clients = clients;
}

void CNetworkThread::setPort(unsigned int port)
{
	if(isRunning())
	{
		stop();
		m_Port = port;
		//and do some other things, like clearing the player list
		start();
	}
	else
	{
		m_Port = port;
		//and do some other things, like clearing the player list
	}
}

void CNetworkThread::threadFunc()
{
	//this should be set before starting:
	if(m_Clients == NULL) return;
	
	m_Net = new CStuntsNet(m_Port);

	while(!m_terminate)
	{
		usleep(10);

		if(m_Net->receiveData())
			for(unsigned int i=0; i < m_Net->m_ReceivedData.size(); i++)
				processMessage(m_Net->m_ReceivedData[i]);
	}

	delete m_Net;
	m_Net = NULL;
}

void CNetworkThread::processMessage(const CMessageBuffer &buffer)
{
	int ID = identify(buffer.getIP(), buffer.getPort());
	if(ID < 0) //unknown
	{
		//Maybe it's a JOIN request
		if(buffer.getType() == CMessageBuffer::textMessage
			|| buffer.getType() == CMessageBuffer::chat //debugging
			)
		{
			CTextMessage t;
			t.setBuffer(buffer);
			if(t.m_Message == "JOIN")
				addClient(buffer.getIP(), buffer.getPort());
		}
	
		return;
	}

	switch(buffer.getType())
	{
		/*
		badMessage = 0,
		dummyMessage = 1,
		movingObject = 3,
		movObjInput = 4,
		car = 5,
		carInput = 6,
		*/
		case CMessageBuffer::objectChoice:
			printf("Received an objectChoice from client %d\n", ID);
			break;
		case CMessageBuffer::chat: //debugging
		case CMessageBuffer::textMessage:
		{
			CTextMessage t;
			t.setBuffer(buffer);
			processMessage(ID, t.m_Message);
		}
			break;
		/*
		case CMessageBuffer::chat:
		{
			CChatMessage c;
			c.setBuffer(buffer);
			printf("Message from %s: %s\n",
				buffer.getIP().toString().c_str(),
				c.m_Message.c_str());
			break;
		}
		*/
		default:
			printf("Unknown Message type received:\n");
			printf("  IP number = %s\n", buffer.getIP().toString().c_str());
			printf("  Port number = %d\n", buffer.getPort());
			printf("  Raw data:\n%s\n", buffer.dump().c_str());
			printf("  type nr = %d\n", buffer.getType());
	}
}

int CNetworkThread::identify(const CIPNumber &ip, unsigned int port)
{
	m_Clients->enter();
	for(unsigned int i=0; i < m_Clients->size(); i++)
	{
		CClient &c = m_Clients->operator[](i);
		if(c.ip == ip && c.port == port)
		{
			m_Clients->leave();
			return i;
		}
	}
	m_Clients->leave();

	return -1;
}

void CNetworkThread::addClient(const CIPNumber &ip, unsigned int port)
{
	CClient c;
	c.ip = ip;
	c.port = port;
	m_Clients->enter();
	m_Clients->push_back(c);
	m_Clients->leave();
	printf("Client added: %s:%d\n", ip.toString().c_str(), port);
}

void CNetworkThread::processMessage(int ID, const CString &message)
{
	printf("TextMessage from client %d: %s\n",
		ID, message.c_str());
}
