/***************************************************************************
                          stuntsnet.cpp  -  UDP + reliability layer
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

#include <cstdio>

#include "stuntsnet.h"

#include "confirmation.h"
#include "textmessage.h"

CStuntsNet::CStuntsNet(unsigned int port) : CUDPNet(port)
{
}

CStuntsNet::~CStuntsNet()
{
}

int CStuntsNet::sendDataReliable(CMessageBuffer &data)
{
	//printf("Sending reliable data\n");
	//please return confirmation
	data.setAC(1);

	//send it
	if(!sendData(data)) return -1;

	while(true)
	{

		//10 sec timeout for confirmation
		CMessageBuffer *mbuf = receiveExpectedData(CMessageBuffer::confirmation, 10000);
		if(mbuf == NULL) return -1; //nothing received for a long time

		//printf("received some kind of confirmation\n");
		CConfirmation c;
		c.setBuffer(*mbuf);
		delete mbuf;

		if(c.m_Counter == data.getCounter() && c.m_MessageType == data.getType())
		{
			//printf("It's the right confirmation\n");
			return c.m_ReturnValue;
		}
	}

	return -1; //we'll never reach this anyway
}

bool CStuntsNet::sendConfirmation(const CMessageBuffer &buf, Uint8 returnValue)
{
	CConfirmation c;
	c.m_Counter = buf.getCounter();
	c.m_MessageType = buf.getType();
	c.m_ReturnValue = returnValue;

	CMessageBuffer cbuf = c.getBuffer();
	cbuf.setIP(buf.getIP());
	cbuf.setPort(buf.getPort());

	//printf("Sending confirmation %d to %s port %d\n",
	//	returnValue, cbuf.getIP().toString().c_str(), cbuf.getPort());

	return sendData(cbuf);
}

CMessageBuffer *CStuntsNet::receiveExpectedData(CMessageBuffer::eMessageType type, unsigned int millisec)
{
	//printf("Expecting a certain kind of information\n");

	while(true)
	{
		//printf("Is it in the buffer?...\n");
		for(unsigned int i=0; i < m_ReceiveBuffer.size(); i++)
		{
			if(m_ReceiveBuffer[i].getType() == type)
			{
				//printf("yes\n");
				CMessageBuffer *ret = new CMessageBuffer;
				ret->operator=(m_ReceiveBuffer[i]);
				m_ReceiveBuffer.erase(m_ReceiveBuffer.begin() + i);
				return ret;
			}
		}
		//printf("no\n");

		//printf("Waiting for new data\n");
		if(!receiveData(millisec)) return NULL; //no data for a long time
	}

	return NULL; //we'll never reach this anyway
}

int CStuntsNet::sendTextMessage(const CString &msg)
{
	CTextMessage tm;
	tm.m_Message = msg;
	CMessageBuffer buf = tm.getBuffer();

	return sendDataReliable(buf);
}
