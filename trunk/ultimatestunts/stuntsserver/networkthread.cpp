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

#include "main.h"
#include "usmacros.h"
#include "netmessages.h"

#include "networkthread.h"

CNetworkThread::CNetworkThread()
{
	//defaults:
	m_Port = DEFAULTPORT;
	m_ServerName = "Ultimate Stunts";
}

CNetworkThread::~CNetworkThread()
{
	stop();
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

void CNetworkThread::setServerName(const CString &servername)
{
	//TODO: some locking for thread safety
	m_ServerName = servername;
}

void CNetworkThread::sendToClient(const CMessageBuffer &b, unsigned int ID)
{
	Clients.enter();
	CIPNumber ip = Clients[ID].ip;
	unsigned int port = Clients[ID].port;
	Clients.leave();

	m_OutputBuffer.enter();
	m_OutputBuffer.push_back(b);
	CMessageBuffer &breal = m_OutputBuffer.back();
	breal.setIP(ip);
	breal.setPort(port);
	m_OutputBuffer.leave();
}

void CNetworkThread::sendToPlayer(const CMessageBuffer &b, unsigned int ID)
{
	//TODO
}

void CNetworkThread::sendToAll(const CMessageBuffer &b)
{
	Clients.enter();
	m_OutputBuffer.enter();
	for(unsigned int ID=0; ID < Clients.size(); ID++)
	{
		CIPNumber ip = Clients[ID].ip;
		unsigned int port = Clients[ID].port;

		m_OutputBuffer.push_back(b);
		CMessageBuffer &breal = m_OutputBuffer.back();
		breal.setIP(ip);
		breal.setPort(port);
	}
	m_OutputBuffer.leave();
	Clients.leave();
}

void CNetworkThread::sendToClient(const CString &s, unsigned int ID)
{
	CTextMessage tm;
	tm.m_Message = s;
	CMessageBuffer b = tm.getBuffer();
	sendToClient(b, ID);
}

void CNetworkThread::sendToPlayer(const CString &s, unsigned int ID)
{
	CTextMessage tm;
	tm.m_Message = s;
	CMessageBuffer b = tm.getBuffer();
	sendToPlayer(b, ID);
}

void CNetworkThread::sendToAll(const CString &s)
{
	CTextMessage tm;
	tm.m_Message = s;
	CMessageBuffer b = tm.getBuffer();
	sendToAll(b);
}


void CNetworkThread::threadFunc()
{
	m_Net = new CStuntsNet(m_Port);

	while(!m_terminate)
	{
		usleep(10000); //< 100 fps

		//FPS:
		float dt = m_Timer.getdt(0.00001);
		float fpsnu = 1.0 / dt;
		m_FPS = 0.9 * m_FPS + 0.1 * fpsnu;

		//File transfer:
		if(m_Net->getReliableQueueSize() < 128)
			for(unsigned int i=0; i < 8; i++)
				m_UploadManager.sendNextChunk();

		//send data
		m_OutputBuffer.enter();
		for(unsigned int i=0; i < m_OutputBuffer.size(); i++)
			if(m_OutputBuffer[i].getAC() != 0)
				{m_Net->sendDataReliable(m_OutputBuffer[i], false);} //reliable, but don't wait
			else
				{m_Net->sendData(m_OutputBuffer[i]);} //unreliable
		m_OutputBuffer.clear();
		m_OutputBuffer.leave();

		//Resending reliable data, if necessary
		m_Net->resendUnconfirmed();

		//receive data
		if(m_Net->receiveData())
		{
			for(unsigned int i=0; i < m_Net->m_ReceiveBuffer.size(); i++)
			{
				CMessageBuffer &b = m_Net->m_ReceiveBuffer[i];
				Uint8 ret = processMessage(b);
				if(b.getAC() != 0)
					m_Net->sendConfirmation(b, ret);
			}

			m_Net->m_ReceiveBuffer.clear();
		}
	}

	delete m_Net;
	m_Net = NULL;
}

Uint8 CNetworkThread::processMessage(const CMessageBuffer &buffer)
{
	int ID = identify(buffer.getIP(), buffer.getPort());
	if(ID < 0) //unknown
	{
		//Some commands can be sent by unknown clients
		if(buffer.getType() == CMessageBuffer::textMessage
			|| buffer.getType() == CMessageBuffer::chat //debugging
			)
		{
			CTextMessage t;
			t.setBuffer(buffer);
			if(t.m_Message == USNET_JOIN)
			{
				addClient(buffer.getIP(), buffer.getPort());
				return 0;
			}

			if(t.m_Message == USNET_SEARCHFORSTUNTS_VERSION) //a broadcast message searching for a server
			{
				//send a reply
				CTextMessage t2;
				t2.m_Message = USNET_ULTIMATESTUNTS_SERVER + m_ServerName;
				CMessageBuffer b2 = t2.getBuffer();
				b2.setAC(1);
				b2.setIP(buffer.getIP());
				b2.setPort(buffer.getPort());
				m_Net->sendDataReliable(b2);

				return 0;
			}
		}
	
		return 1;
	}

	switch(buffer.getType())
	{
		/*
		Unsupported:
		badMessage
		dummyMessage
		movingObject
		movObjInput
		car
		confirmation
		fileChunk
		*/
		case CMessageBuffer::movObjInput:
			gamecorethread.processInput(buffer);
			return 0;
			break;
		case CMessageBuffer::objectChoice:
		{
			consolethread.write(CString("New player request from client ") + ID);
			CObjectChoice oc;
			oc.setBuffer(buffer);
			int ret = Clients.addRequest_safe(ID, oc);

			if(Clients.reachedMinimum_safe())
				gamecorethread.GO_minPlayers(); //giving permission to start the game
			
			if(ret < 0) return OBJECTCHOICE_REFUSED;
			return ret;
		}
			break;
		case CMessageBuffer::textMessage:
		{
			CTextMessage t;
			t.setBuffer(buffer);
			return processMessage(ID, t.m_Message);
		}
			break;
		case CMessageBuffer::chat:
		{
			CChatMessage c;
			c.setBuffer(buffer);
			consolethread.write(CString("Chat Message from ") + ID + c.m_Message.c_str());
			break;
		}
		default:
			consolethread.write(CString() +
				"Unknown Message type received:\n" +
				"  IP number = " + buffer.getIP().toString() + "\n"
				"  Port number = " + buffer.getPort() + "\n"
				"  Raw data:\n" + buffer.dump() + "\n"
				"  type nr = " + buffer.getType() + "\n");
	}

	return 255;
}

int CNetworkThread::identify(const CIPNumber &ip, unsigned int port)
{
	Clients.enter();
	for(unsigned int i=0; i < Clients.size(); i++)
	{
		CClient &c = Clients[i];
		if(c.ip == ip && c.port == port)
		{
			Clients.leave();
			return i;
		}
	}
	Clients.leave();

	return -1;
}

void CNetworkThread::addClient(const CIPNumber &ip, unsigned int port)
{
	CClient c;
	c.ip = ip;
	c.port = port;
	c.ready = false;
	Clients.enter();
	Clients.push_back(c);
	Clients.leave();
	consolethread.write(CString("Client added: ") + ip.toString() + ":" + port);
}

Uint8 CNetworkThread::processMessage(int ID, const CString &message)
{
	if(message == USNET_PING)
	{
		//consolethread.write("Received PING message");
		//just send the confirmation
		;
	}
	else if(message == USNET_LEAVE)
	{
		Clients.enter();
		Clients.erase(Clients.begin() + ID);
		//players, moving objects etc. still exist, but the input is not updated anymore
		Clients.leave();
	}
	else if(message == USNET_READY)
	{
		Clients.enter();
		Clients[ID].ready = true;
		Clients.leave();
	}
	else if(message == USNET_OBJECTS)
	{
		ObjectChoices.enter();
		for(unsigned int i=0; i < ObjectChoices.size(); i++)
		{
			CMessageBuffer b = ObjectChoices[i].getBuffer();
			sendToClient(b, ID); //puts on the queue
		}
		ObjectChoices.leave();
		CObjectChoice ender;
		ender.m_ObjType = CMessageBuffer::badMessage;
		CMessageBuffer b = ender.getBuffer();
		sendToClient(b, ID); //puts on the queue
	}
	else if(message == USNET_STATUS)
	{
		CTextMessage tm;
		tm.m_Message = USNET_STATUS + gamecorethread.getGameStatus();
		CMessageBuffer b = tm.getBuffer();
		b.setAC(0);
		sendToClient(b, ID); //puts on the queue
	}
	else if(message.mid(0, CString(USNET_GET).length()) == USNET_GET)
	{
		CString filename = message.mid(CString(USNET_GET).length());
		return m_UploadManager.addFileRequest(ID, filename); //return: do we have the file?
	}
	else
	{
		printf("TextMessage from client %d: %s\n",
			ID, message.c_str());
	}

	return 0;
}
