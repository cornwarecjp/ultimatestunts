/***************************************************************************
                          clientnet.h  -  Client-side network class
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

#ifndef CLIENTNET_H
#define CLIENTNET_H

#include "usmacros.h"

#include "cstring.h"
#include "stuntsnet.h"
#include "ipnumber.h"
#include "netfilecontrol.h"

/**
  *@author CJP
  */

class CClientNet : public CStuntsNet {
public: 
	CClientNet(CString host="localhost", int port=DEFAULTPORT);
	virtual ~CClientNet();

	virtual bool isConnected();

	//Replacements that only send to host:port
	virtual bool receiveData(unsigned int millisec=1);
	virtual bool sendData(CMessageBuffer &data); //NOT const: it sets IP number and port

	//get server info
	CString getHost(){return m_Hostname;}
	unsigned int getPort(){return m_ServerPort;}

	//Protocol tools
	bool sendReady();
	bool wait4Ready();

	struct SBroadcastResult
	{
		CString serverName;
		CString hostName;
		int port;
	};

	static vector<SBroadcastResult> broadcast(int port);
	
protected:
	CString m_Hostname;
	CIPNumber m_ServerIP;
	int m_ServerPort;

private:
	CNetFileControl m_FileCtl;
};

#endif
