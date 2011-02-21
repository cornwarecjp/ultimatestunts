/***************************************************************************
                          chatmessage.h  -  A chat message to all players
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

#ifndef CHATMESSAGE_H
#define CHATMESSAGE_H

#include "textmessage.h"

/**
  *@author CJP
  */

class CChatMessage : public CTextMessage  {
public:
	CChatMessage(const CString &str);
	CChatMessage();
	virtual ~CChatMessage();

	//For network transfer & other stuff
	virtual CBinBuffer &getData(CBinBuffer &b) const;            // returns class data as binbuffer
	virtual bool setData(const CBinBuffer &b, unsigned int &pos);// rebuild class data from binbuffer

	virtual CMessageBuffer::eMessageType getType() const
		{return CMessageBuffer::chat;}

	float m_SendTime; //moment of creation (relative to start of the game)
	int m_ToMovingObject; //who should receive it, -1 = all
};

#endif
