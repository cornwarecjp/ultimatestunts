/***************************************************************************
                          objectchoice.cpp  -  description
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


#include "binbuffer.h"
#include "objectchoice.h"

CObjectChoice::CObjectChoice()
{
  m_CarNumber = 0;
  this->setType(CMessageBuffer::objectChoice);
}

CObjectChoice::~CObjectChoice()
{}


CBinBuffer & CObjectChoice::getData() const {
  CBinBuffer *res = new CBinBuffer();
  (*res)+=m_CarNumber;
  return (*res);
}

bool CObjectChoice::setData(const CBinBuffer & b) {
  CBinBuffer bb = b;                           // const!
  m_CarNumber = bb.getUint16(0);
  return (true);
}


