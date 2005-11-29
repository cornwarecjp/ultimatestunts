/***************************************************************************
                          chatmessage.cpp  -  A chat message to all players
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

#include "chatmessage.h"
#include "timer.h"
#include "world.h"

CChatMessage::CChatMessage(const CString &str)
{
	m_SendTime = CTimer().getTime() - theWorld->m_GameStartTime;
	m_Message = str;
}

CChatMessage::CChatMessage()
{
	m_SendTime = CTimer().getTime() - theWorld->m_GameStartTime;
}

CChatMessage::~CChatMessage()
{
}

CBinBuffer &CChatMessage::getData(CBinBuffer &b) const
{
	CTextMessage::getData(b);
	b.addFloat32(m_SendTime, 0.005);
	b += (Uint8)(m_ToMovingObject + 1); //lets -1 fall into the unsigned range

	return b;
}

bool CChatMessage::setData(const CBinBuffer &b, unsigned int &pos)
{
	if(!CTextMessage::setData(b, pos)) return false;
	m_SendTime = b.getFloat32(pos, 0.005);
	m_ToMovingObject = b.getUint8(pos) - 1; //lets -1 fall into the unsigned range

	return true;
}
