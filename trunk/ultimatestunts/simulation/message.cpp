/***************************************************************************
                          message.cpp  -  description
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

#include "message.h"

CMessageBuffer & CMessage::getBuffer() {
  CMessageBuffer *res = new CMessageBuffer();

  this->m_Data.setData(this->getData());
  *res = m_Data  ;
  return (*res);
}
 	
bool CMessage::setBuffer(const CMessageBuffer & b) {
 m_Data = b;
 this->setData(m_Data.getData());
 return (true);
}

void CMessage::setType(const CMessageBuffer::eMessageType & t) {
	m_Data.setType(t);
}

void CMessage::setCounter(const Uint16 & counter) {
	m_Data.setCounter(counter);
}

void CMessage::setAC(const bool & ac) {
	if (ac) m_Data.setAC(1); else m_Data.setAC(0);
}


CMessage::CMessage(){
}

CMessage::CMessage(const CMessageBuffer & mb)
{
 this->setBuffer(mb);
}

CMessage::~CMessage(){
}

