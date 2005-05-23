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
