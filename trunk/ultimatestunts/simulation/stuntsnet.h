/***************************************************************************
                          stuntsnet.h  -  Basic network functions
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

#ifndef STUNTSNET_H
#define STUNTSNET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <vector>
namespace std {}
using namespace std;

#include "messagebuffer.h"
#include "usmacros.h"

/**
  *@author CJP
  */

class CStuntsNet {
public: 
	CStuntsNet(unsigned int port); //0 = take a free port (like for clients)
	virtual ~CStuntsNet();

	virtual bool receiveData();
	vector<CMessageBuffer> m_ReceivedData;

	virtual bool sendData(const CMessageBuffer &data);

private:
	int m_Socket;
	struct sockaddr_in m_MyAddress;
};

#endif
