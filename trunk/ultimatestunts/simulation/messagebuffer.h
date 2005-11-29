/***************************************************************************
                          messagebuffer.h  -  Raw message containing type and data
                             -------------------
    begin                : Thu Dec 5 2002
    copyright            : (C) 2002 by bones
    email                : boesemar@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MESSAGEBUFFER_H
#define MESSAGEBUFFER_H


#include "ipnumber.h"
#include "binbuffer.h"
#include "cstring.h"



class CMessageBuffer : public CBinBuffer
{
public:

	struct s_msg_header
	{
		Uint8 tid;		// type id
		Uint8 ac;		// acknowledge
		Uint16 counter; // package counter
	};


	enum eMessageType
	{   // more to add
		badMessage = 0,
		dummyMessage = 1,
		objectChoice = 2,
		movingObject = 3,
		movObjInput = 4,
		car = 5,
		hiscore = 6,
		chat = 7,
		textMessage = 8,
		confirmation = 9,
		fileChunk = 10
	};

	CMessageBuffer(const CMessageBuffer::eMessageType & t);
	CMessageBuffer();
	~CMessageBuffer();

	bool setType(const eMessageType t);
	bool setAC(const Uint8 acflag);
	bool setCounter(const Uint16 counter);

	eMessageType getType() const;
	Uint8 getAC() const;
	Uint16 getCounter() const;


	bool setData(const CBinBuffer & b);
	CBinBuffer getData() const;


	int getHeaderLength() const;

	bool setBuffer(const CBinBuffer & b);               // restore messagebuffer
	CBinBuffer getBuffer();

	void setIP(const CIPNumber & ip) { m_netIP = ip; }
	void setPort(const int port) { m_netPort = port; }
	CIPNumber getIP() const { return m_netIP; }
	int getPort() const { return m_netPort; }

private:
	s_msg_header getHeader() const;
	bool setHeader(s_msg_header &);

	CIPNumber m_netIP;      // to be set to tranfer messages over network
	int m_netPort;        // this can be either source or destination address
};



#endif
