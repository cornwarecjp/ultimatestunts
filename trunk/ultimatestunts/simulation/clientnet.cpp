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

CClientNet::CClientNet(CString host, int port) : CStuntsNet(0)
{
	m_ServerIP = host;
	m_ServerPort = port;
	m_Hostname = host;

	sendTextMessage("JOIN");
}

CClientNet::~CClientNet()
{
	sendTextMessage("LEAVE");
}

bool CClientNet::reveiveData()
{
	if(!CStuntsNet::receiveData())
		return false;

	//TODO (maybe) check if the packages come from the server
	return true;
}

bool CClientNet::sendData(CMessageBuffer &data)
{
	data.setIP(m_ServerIP);
	data.setPort(m_ServerPort);
	return CStuntsNet::sendData(data);
}

bool CClientNet::sendTextMessage(const CString &msg)
{
	CTextMessage tm;
	tm.m_Message = msg;
	CMessageBuffer buf = tm.getBuffer();

	//TODO: make it a loop, and wait for confirmation
	return sendData(buf);
}
