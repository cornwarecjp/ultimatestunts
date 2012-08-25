/***************************************************************************
                          message.h  -  Base-class for messages being sent over the network
                             -------------------
    begin                : Thu Dec 5 2002
    copyright            : (C) 2002 by CJP
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

#ifndef MESSAGE_H
#define MESSAGE_H


/**
  *@author CJP
  */

#include "messagebuffer.h"

class CMessage {
public:
	CMessage();
	CMessage(const CMessageBuffer &);
	virtual ~CMessage();


	//Class data as binbuffer
	virtual CBinBuffer &getData(CBinBuffer &) const = 0; //TODO: make this a little faster
	virtual bool setData(const CBinBuffer &, unsigned int &pos) = 0;

	//Class data plus network header
	CMessageBuffer getBuffer();
	bool setBuffer(const CMessageBuffer &);

	// header wrappers
	//void setType(const CMessageBuffer::eMessageType & t);
	//void setCounter(const Uint16 & counter);
	//void setAC(const bool & ac);
	//Uint16 getCounter() const { return (m_Data.getCounter()); }
	//Uint8 getAC() const { return (m_Data.getAC()); }

	virtual CMessageBuffer::eMessageType getType() const
		{return CMessageBuffer::badMessage; } //you should reimplement this in derived classes

};
#endif
