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
  s_msg_header h;
  setHeader(h);
}
CMessageBuffer::~CMessageBuffer(){
}


bool CMessageBuffer::setType(const eMessageType t){
  s_msg_header *m = this->getHeader();
  m->tid = (Uint8) t;
  return (setHeader(*m));
}

bool CMessageBuffer::setAC(const Uint8 acflag) {
  s_msg_header *m = getHeader();
  m->ac = (Uint8) acflag;
  return (setHeader(*m));
}

bool CMessageBuffer::setCounter(const Uint16 counter) {
  s_msg_header *m = getHeader();
  m->counter =  counter;
  return (setHeader(*m));
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


CMessageBuffer::s_msg_header *CMessageBuffer::getHeader() const
{
  s_msg_header *res =  new s_msg_header;
  int newpos;
  if (this->size() < sizeof(s_msg_header)) return (NULL);
  res->tid = this->getUint8(0, &newpos);
  res->ac = this->getUint8(newpos, &newpos);
  res->counter = this->getUint16(newpos);
  return (res);
}

CMessageBuffer::eMessageType CMessageBuffer::getType() const {
 s_msg_header *m = getHeader();
 if (m == NULL) return(badMessage);
 return ((CMessageBuffer::eMessageType) m->tid);
}

Uint8 CMessageBuffer::getAC() const {
 s_msg_header *m = getHeader();
 if (m == NULL) return(badMessage);
 return (m->ac);
}

Uint16 CMessageBuffer::getCounter() const {
 s_msg_header *m = getHeader();
 if (m == NULL) return(badMessage);
 return (m->counter);
}



bool CMessageBuffer::setHeader(s_msg_header & h) {
   CBinBuffer tmp;
   tmp.empty();
   tmp+=h.tid;
   tmp+=h.ac;
   tmp+=h.counter;
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
  s_msg_header h;
  h.tid = (Uint8) t;
  setHeader(h);
}


int CMessageBuffer::getHeaderLength() const
{
  return (sizeof(s_msg_header));
}
