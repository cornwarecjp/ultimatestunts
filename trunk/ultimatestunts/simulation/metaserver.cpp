/***************************************************************************
                          metaserver.cpp  -  Connect to a metaserver for finding game servers
                             -------------------
    begin                : do jul 5 2007
    copyright            : (C) 2007 by CJP
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

#include "lconfig.h"

#include "metaserver.h"

CMetaServer::CMetaServer()
{
	m_Hostname = theMainConfig->getValue("misc", "metaserver_hostname");
	m_URI = theMainConfig->getValue("misc", "metaserver_filename");
	m_Port = theMainConfig->getValue("misc", "metaserver_port").toInt();

	m_Connected = false;
}


CMetaServer::~CMetaServer()
{
	if(m_Connected) tcpDisconnect();
}


//Public API:
vector<CClientNet::SBroadcastResult> CMetaServer::getServerList()
{
	printf("getServerList()\n");

	vector<CClientNet::SBroadcastResult> ret;

	if(!tcpConnect()) return ret; //TODO: show error message
	if(!httpGet("")) return ret; //TODO: show error message
	CString response = httpReceiveResponse();
	tcpDisconnect();

	//TODO: parse response

	CClientNet::SBroadcastResult s;

	s.serverName = "Foo";
	s.hostName = "127.0.0.1";
	s.port = 4301;
	ret.push_back(s);

	s.serverName = "Bar";
	s.hostName = "192.168.10.2";
	s.port = 4301;
	ret.push_back(s);

	return ret;
}

bool CMetaServer::setServer(const CString &name, unsigned int port)
{
	if(!tcpConnect()) return false;

	printf(
		"Setting server: name %s, port %d\n",
		name.c_str(), port);

	tcpDisconnect();

	return true;
}

void CMetaServer::removeServer()
{
	if(!tcpConnect()) return;

	printf("Removing server\n");

	tcpDisconnect();
}




//HTTP-level
bool CMetaServer::httpGet(const CString &args)
{
	return writeStr(
		CString("GET ") + m_URI+args + " HTTP/1.1\r\n"
		"Host: " + m_Hostname + "\r\n"
		"Connection: Close\r\n"
		"\r\n"
		);
}

bool CMetaServer::httpPost(const CString &args, const CString &data)
{
	//NYI
	return false;
}

CString CMetaServer::httpReceiveResponse()
{
	//NYI
	return "";
}


//Read-and-write-utilities
CString CMetaServer::readLine()
{
	CString ret;

	while(true)
	{
		int c = readChar();
		if(c == '\n' || c < 0) break; //stop
		if(c == '\r') continue; //ignore

		ret += (unsigned char)c;
	}

	return ret;
}

int CMetaServer::readChar()
{
	//NYI
	return -1;
}

CString CMetaServer::readStr(unsigned int maxlen)
{
	//NYI
	return "";
}

bool CMetaServer::writeStr(const CString &data)
{
	//NYI
	return true;
}


//TCP-level
bool CMetaServer::tcpConnect()
{
	//Disconnect previous connection
	if(m_Connected) tcpDisconnect();

	//NYI

	m_Connected = true;
	return true;
}

void CMetaServer::tcpDisconnect()
{
	//TODO: uncomment
	//close(m_SockFD);
	m_Connected = false;
}

