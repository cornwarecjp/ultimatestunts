/***************************************************************************
                          clientnet.cpp  -  Client-side network class
                             -------------------
    begin                : vr jan 31 2003
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

#include "clientnet.h"
#include "textmessage.h"
#include "timer.h"
#include "netmessages.h"

//Network testing:
//unsigned int inBytes = 0, outBytes = 0;
//float startTime = 0.0;

CClientNet::CClientNet(CString host, int port) : CStuntsNet(0)
{
	m_ServerIP = host;
	m_ServerPort = port;
	m_Hostname = host;

	sendTextMessage(USNET_JOIN);
	//startTime = CTimer().getTime();

	m_FileCtl.setNetwork(this);
}

CClientNet::~CClientNet()
{
	sendTextMessage(USNET_LEAVE);
}

bool CClientNet::isConnected()
{
	if(!CStuntsNet::isConnected()) return false;

	if(sendTextMessage(USNET_PING) < 0)
	{
		closeConnection(); //to make other isConnected calls faster (no timeout waiting)
		return false;
	}

	return true;
}

bool CClientNet::receiveData(unsigned int millisec)
{
	//unsigned int prevN = m_ReceiveBuffer.size();

	if(!CStuntsNet::receiveData(millisec))
		return false;

	//TODO (maybe) check if the packages come from the server

	//for(unsigned int i=prevN; i < m_ReceiveBuffer.size(); i++)
	//	inBytes += m_ReceiveBuffer[i].size();

	return true;
}

bool CClientNet::sendData(CMessageBuffer &data)
{
	data.setIP(m_ServerIP);
	data.setPort(m_ServerPort);

	/*
	outBytes += data.size();

	float dt = CTimer().getTime() - startTime;
	float inBPS = inBytes / dt;
	float outBPS = outBytes / dt;
	printf("In %.2f kBPS Out %.2f kBPS\n", inBPS/1000.0, outBPS/1000.0);
	*/

	return CStuntsNet::sendData(data);
}

bool CClientNet::sendReady()
{
	return CStuntsNet::sendTextMessage(USNET_READY) == 0;
}

bool CClientNet::wait4Ready()
{
	while(true)
	{
		CMessageBuffer *b = CStuntsNet::receiveExpectedData(CMessageBuffer::textMessage, 60000);
		if(b == NULL) return false;

		CTextMessage tm;
		tm.setBuffer(*b);
		delete b;

		if(tm.m_Message == USNET_READY)
			return true;
	}

	return false;
}

vector<CClientNet::SBroadcastResult> CClientNet::broadcast(int port)
{
	vector<CClientNet::SBroadcastResult> ret;

	CTextMessage tm;
	tm.m_Message = USNET_SEARCHFORSTUNTS_VERSION;
	CMessageBuffer sendbuf = tm.getBuffer();
	sendbuf.setAC(0);
	sendbuf.setPort(port);
	sendbuf.setIP(getBroadcastAddress());

	//Temporary network for broadcasting purpose
	CStuntsNet tmpnet(0);
	tmpnet.sendData(sendbuf);

	while(true)
	{
		printf("Receiving (again)...\n");
		if(!(tmpnet.receiveData(3000)) ) //we don't expect big lag times on a LAN
		{
			printf("No more data after 3 seconds\n");
			return ret; //no server found
		}

		for(unsigned int i=0; i < tmpnet.m_ReceiveBuffer.size(); i++)
		{
			CMessageBuffer &buf = tmpnet.m_ReceiveBuffer[i];
			if(buf.getAC() != 0)
				tmpnet.sendConfirmation(buf, 0);

			if(tmpnet.m_ReceiveBuffer[i].getType() == CMessageBuffer::textMessage)
			{
				CTextMessage tm2;
				tm2.setBuffer(buf);
				CString msg = tm2.m_Message;

				CString smsg = USNET_ULTIMATESTUNTS_SERVER;
				if(msg.mid(0, smsg.length()) == smsg) //it is a response to our broadcast
				{
					CClientNet::SBroadcastResult br;
					br.serverName = msg.mid(smsg.length());
					br.hostName = buf.getIP().toString();
					br.port = port;

					//Now, erase it from the buffer
					tmpnet.m_ReceiveBuffer.erase(tmpnet.m_ReceiveBuffer.begin()+i);
					i--; //check the same index again

					//first check if we don't already know this server
					for(unsigned int j=0; j < ret.size(); j++)
						if(ret[j].hostName == br.hostName)
							continue;

					//else, add it to our list
					ret.push_back(br);

					printf("Found %s on %s\n", br.serverName.c_str(), br.hostName.c_str());
				}
			}
		}

		//Send the broadcast again
		//tmpnet.sendData(sendbuf);
	}

	return ret;
}
