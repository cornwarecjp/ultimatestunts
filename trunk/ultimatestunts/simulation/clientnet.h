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

#include "cstring.h"
#include "stuntsnet.h"
#include "ipnumber.h"

/**
  *@author CJP
  */

class CClientNet : protected CStuntsNet { //TODO: derive from CFileCtrl
public: 
	CClientNet(CString host="localhost", int port=1500);
	virtual ~CClientNet();

	virtual bool reveiveData();
	vector<CMessageBuffer> m_ReceivedData;

	virtual bool sendData(CMessageBuffer &data); //NOT const: it sets IP number and port

	bool sendTextMessage(const CString &msg);

	CString getHost(){return m_Hostname;}
	unsigned int getPort(){return m_ServerPort;}
protected:
	CString m_Hostname;
	CIPNumber m_ServerIP;
	int m_ServerPort;

};

#endif
