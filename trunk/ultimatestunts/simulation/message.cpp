/***************************************************************************
                          message.cpp  -  Base-class for messages being sent over the network
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

#include <cstdio>

#include "message.h"

CMessage::CMessage()
{}

CMessage::CMessage(const CMessageBuffer & mb)
{
	setBuffer(mb);
}

CMessage::~CMessage()
{}

CMessageBuffer CMessage::getBuffer()
{
	CMessageBuffer ret;
	CBinBuffer b;
	ret.setType(getType());
	//ret.setAC(getAC());
	//ret.setCounter(getCounter());
	ret.setData(getData(b));
	return ret;
}
 	
bool CMessage::setBuffer(const CMessageBuffer & b)
{
	//TODO: check buffer type ID
	unsigned int pos = b.getHeaderLength();
	return setData(b, pos);
}

/*
void CMessage::setType(const CMessageBuffer::eMessageType & t)
{
	m_Data.setType(t);
}

void CMessage::setCounter(const Uint16 & counter) {
	m_Data.setCounter(counter);
}

void CMessage::setAC(const bool & ac) {
	if (ac) m_Data.setAC(1); else m_Data.setAC(0);
}
*/

