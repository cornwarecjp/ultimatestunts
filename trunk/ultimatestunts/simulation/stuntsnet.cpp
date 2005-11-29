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
#include <unistd.h>

#include "stuntsnet.h"

#include "textmessage.h"
#include "confirmation.h"
#include "timer.h"

CStuntsNet::CStuntsNet(unsigned int port) : CUDPNet(port)
{
}

CStuntsNet::~CStuntsNet()
{
}

int CStuntsNet::sendDataReliable(CMessageBuffer &data, bool wait)
{
	//printf("Sending reliable data\n");
	//please return confirmation
	data.setAC(1);

	//send it
	if(!sendData(data)) return -1;

	m_UnconfirmedMessages.push_back(data);

	//if(!wait)
	//	printf("Reliable transmission started of type=%d counter=%d\n", data.getType(), data.getCounter());

	if(wait)
	{
		//printf("Waiting for confirmation of type=%d counter=%d\n", data.getType(), data.getCounter());
		m_ReturnCheckMessage.setCounter(data.getCounter());
		m_ReturnCheckMessage.setType(data.getType());
		m_ReturnFound = false;

		int waitcounter = 0;
		while(m_UnconfirmedMessages.size() > 0 && waitcounter < 5000) //timeout of 5 seconds
		{
			waitcounter++;
			receiveData(1); //wait 1 millisecond for data

			resendUnconfirmed();

			if(m_ReturnFound)
			{
				//printf("Received the right confirmation\n");
				return m_ReturnValue;
			}
			
		}
		if(waitcounter >= 5000)
		{
			printf("Error: connection timeout!!\n");
			m_UnconfirmedMessages.clear(); //give up all messages
		}
	}

	return 0; //return value for non-waiting reliable messages
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

void CStuntsNet::resendUnconfirmed()
{
	if(m_UnconfirmedMessages.size() == 0) return;

	processConfirmations(); //to make sure that we need to resend anything

	//TODO
}

bool CStuntsNet::receiveData(unsigned int millisec)
{
	processConfirmations(); //to make sure that we can compare sizes without problems
	unsigned int prevSize = m_ReceiveBuffer.size();

	if(!CUDPNet::receiveData(millisec)) return false; //no new data

	processConfirmations();

	/*
	if(m_UnconfirmedMessages.size() > 0)
	{
		printf("Unconfirmed messages: %d\n", m_UnconfirmedMessages.size());

		for(unsigned int i=0; i < m_UnconfirmedMessages.size(); i++)
			printf("  Unconfirmed type=%d counter=%d\n",
				m_UnconfirmedMessages[i].getType(), m_UnconfirmedMessages[i].getCounter());
	}

	if(m_Confirmations.size() > 0)
		printf("Unused confirmations: %d\n", m_Confirmations.size());
	*/

	//already send confirmations here, to prevent that both sides wait on each other
	//on the other hand, we do not yet know the return value
	//sendConfirmations();

	return m_ReceiveBuffer.size() > prevSize; //only true when non-confirmation messages have arrived
}

void CStuntsNet::processConfirmations()
{
	//Search in the receive buffer for confirmations
	for(int i=0; i < int(m_ReceiveBuffer.size()); i++)
	{
		if(m_ReceiveBuffer[i].getType() == CMessageBuffer::confirmation)
		{
			CConfirmation c;
			c.setBuffer(m_ReceiveBuffer[i]);

			m_ReceiveBuffer.erase(m_ReceiveBuffer.begin() + i);
			i--; //check the same index again


			//First, set return value if necessary
			if(c.m_Counter == m_ReturnCheckMessage.getCounter() && c.m_MessageType == m_ReturnCheckMessage.getType())
			{
				//printf("Received the message return value\n");
				m_ReturnFound = true;
				m_ReturnValue = c.m_ReturnValue;
			}
	
			//Then, remove element from the unconfirmed list that is confirmed
			bool recognised = false;
			for(unsigned int j=0; j < m_UnconfirmedMessages.size(); j++)
			{
				CMessageBuffer b = m_UnconfirmedMessages[j];
				if(c.m_Counter == b.getCounter() && c.m_MessageType == b.getType()) //the right confirmation
				{
					//printf("Received a confirmation for type=%d counter=%d\n", c.m_MessageType, c.m_Counter);
					m_UnconfirmedMessages.erase(m_UnconfirmedMessages.begin() + j);

					recognised = true;
					break;
				}
			}

			if(!recognised)
				printf("Warning: unrecognised confirmation of type %d, counter = %d\n", c.m_MessageType, c.m_Counter);
		}
	}

}

void CStuntsNet::sendConfirmations()
{
	for(int i=0; i < int(m_ReceiveBuffer.size()); i++)
		if(m_ReceiveBuffer[i].getAC() != 0)
		{
			sendConfirmation(m_ReceiveBuffer[i], 0); //default return value
			m_ReceiveBuffer[i].setAC(0); //we don't need to send a new confirmation
		}
}

CMessageBuffer *CStuntsNet::receiveExpectedData(CMessageBuffer::eMessageType type, unsigned int millisec)
{
	//printf("Expecting a certain kind of information\n");
	CTimer timer;
	float t = timer.getTime();

	while(1000*(timer.getTime() - t) < millisec)
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
