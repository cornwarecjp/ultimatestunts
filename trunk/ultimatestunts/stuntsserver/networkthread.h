/***************************************************************************
                          networkthread.h  -  Listening to messages from clients
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

#ifndef NETWORKTHREAD_H
#define NETWORKTHREAD_H

#include "bthread.h"
#include "stuntsnet.h"
#include "client.h"

/**
  *@author CJP
  */

class CNetworkThread : public CBThread  {
public: 
	CNetworkThread(unsigned int port=1500);
	virtual ~CNetworkThread();

	void giveClientList(CClientList *clients);

	void setPort(unsigned int port);
	unsigned int getPort()
		{return m_Port;}

	virtual void threadFunc();
protected:
	unsigned int m_Port;
	CStuntsNet *m_Net;

	CClientList *m_Clients;
	int identify(const CIPNumber &ip, unsigned int port);
	void addClient(const CIPNumber &ip, unsigned int port);

	void processMessage(const CMessageBuffer &buffer);
	void processMessage(int ID, const CString &message);
};

#endif
