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

#include "objectchoice.h"

CObjectChoice::CObjectChoice()
{m_CarNumber = 0;}

CObjectChoice::~CObjectChoice()
{}

CObjectChoice::CObjectChoice(CMessageBuffer b)
{
  setBuffer(b);
}

CMessageBuffer CObjectChoice::getBuffer()
{
  CMessageBuffer b;
  //add m_CarNumber to b
  return b;
}

void CObjectChoice::setBuffer(CMessageBuffer b)
{
  //get m_CarNumber from b
}
