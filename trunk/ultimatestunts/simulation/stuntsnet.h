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

#include "udpnet.h"
  
class CStuntsNet : public CUDPNet {
public: 
	CStuntsNet(unsigned int port); //0 = take a free port (like for clients)
	virtual ~CStuntsNet();

	//Reliability functions:
	virtual int sendDataReliable(CMessageBuffer &data);
	virtual bool sendConfirmation(const CMessageBuffer &buf, Uint8 returnValue);

	//useful tools:
	CMessageBuffer *receiveExpectedData(CMessageBuffer::eMessageType type, unsigned int millisec);
	int sendTextMessage(const CString &msg);
};

#endif
