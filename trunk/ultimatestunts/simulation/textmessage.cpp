/***************************************************************************
                          textmessage.cpp  -  A simple text message
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

#include "textmessage.h"

CTextMessage::CTextMessage(){
}
CTextMessage::~CTextMessage(){
}

CBinBuffer &CTextMessage::getData(CBinBuffer &b) const
{
	b += m_Message;
	return b;
}

bool CTextMessage::setData(const CBinBuffer &b, unsigned int &pos)
{
	m_Message = b.getCString(pos);
	return true;
}
