/***************************************************************************
                          messagebuffer.cpp  -  Raw message containing type and data
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


/*
   ---
   header
   -----
   Uint8 type id
   Uint8 Acknowledge flag
   Uint16 counter

   ----
   data
   ----
   var length


   header + data = buffer

*/

#include "messagebuffer.h"
#include "messagebuffer.h"


CMessageBuffer::CMessageBuffer(){
}
CMessageBuffer::~CMessageBuffer(){
}


bool CMessageBuffer::setType(const eMessageType t){
  this->m_HeaderCp.tid = t;
  return (this->setHeader());
}

bool CMessageBuffer::setAC(const Uint8 acflag) {
  this->m_HeaderCp.ac = acflag;
  return (this->setHeader());
}

bool CMessageBuffer::setCounter(const Uint16 counter) {
  this->m_HeaderCp.counter = counter;
  return (this->setHeader());
}


bool CMessageBuffer::setData(const CBinBuffer & b) {
  this->resize(b.size() + this->getHeaderLength());
  for (int i=0;i<b.size();i++) { (*this)[this->getHeaderLength()+i] = b[i]; }
  return (true);
}

CBinBuffer & CMessageBuffer::getData() const
{
  CBinBuffer *res = new CBinBuffer();
  (*res) = this->substr(this->getHeaderLength());
  return (*res);
}


bool CMessageBuffer::rereadHeader() {
  if (this->size() < sizeof(s_msg_header)) return (false);
  m_HeaderCp.tid = this->getUint8(0);
  m_HeaderCp.ac = this->getUint8(this->getNewCursor());
  m_HeaderCp.counter = this->getUint16(this->getNewCursor());
  return (true);
}

CMessageBuffer::eMessageType CMessageBuffer::getType() {
 rereadHeader();
 return ((eMessageType) m_HeaderCp.tid);
}

Uint8 CMessageBuffer::getAC() {
 rereadHeader();
 return (m_HeaderCp.ac);
}

Uint16 CMessageBuffer::getCounter() {
  rereadHeader();
  return (m_HeaderCp.counter);
}




bool CMessageBuffer::setHeader() {
   CBinBuffer tmp;
   tmp.empty();
   tmp+=m_HeaderCp.tid;
   tmp+=m_HeaderCp.ac;
   tmp+=m_HeaderCp.counter;
   if (this->size() < tmp.size()) { this->resize(tmp.size()); }
   for (int i=0;i<tmp.size();i++) { (*this)[i] = tmp[i]; }
   return (true);
}


bool CMessageBuffer::setBuffer(const CBinBuffer & b)
{
      // fixme: bad code
  if (this->size() < b.size()) { this->resize(b.size()); }
  for (int i=0;i<b.size();i++) { (*this)[i] = b[i]; }
  return (true);
}

CBinBuffer & CMessageBuffer::getBuffer() {
  CBinBuffer *res = new CBinBuffer(*this);
  return (*res);

}


CMessageBuffer::CMessageBuffer(const CMessageBuffer::eMessageType & t){
  m_HeaderCp.tid=t;
  setHeader();
}


int CMessageBuffer::getHeaderLength() const
{
  return (sizeof(s_msg_header));
}
