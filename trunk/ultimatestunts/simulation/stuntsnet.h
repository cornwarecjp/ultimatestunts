/***************************************************************************
                          stuntsnet.h  -  UDP + reliability layer
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

/**
  *@author CJP
  */

#include <vector> //STL vector template
namespace std {}
using namespace std;

#include "udpnet.h"
  
class CStuntsNet : public CUDPNet {
public: 
	CStuntsNet(unsigned int port); //0 = take a free port (like for clients)
	virtual ~CStuntsNet();

	//Reliability functions:
	virtual int sendDataReliable(CMessageBuffer &data, bool wait=true);
	virtual bool sendConfirmation(const CMessageBuffer &buf, Uint8 returnValue);
	void resendUnconfirmed();

	unsigned int getReliableQueueSize()
		{return m_UnconfirmedMessages.size();}

	virtual bool receiveData(unsigned int millisec=1); //overload that catches confirmations

	//useful tools:
	CMessageBuffer *receiveExpectedData(CMessageBuffer::eMessageType type, unsigned int millisec);
	int sendTextMessage(const CString &msg);

protected:
	vector<CMessageBuffer> m_UnconfirmedMessages;

	//Confirmation return value checking
	CMessageBuffer m_ReturnCheckMessage; //check for return value of this
	bool m_ReturnFound;
	int m_ReturnValue;

	void processConfirmations();
	void sendConfirmations(); //send confirmations for all received reliable messages
};

#endif
