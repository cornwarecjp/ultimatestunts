/***************************************************************************
                          confirmation.h  -  Confirming that a package has arrived
                             -------------------
    begin                : ma jan 17 2005
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

#ifndef CONFIRMATION_H
#define CONFIRMATION_H
#include "message.h"

class CConfirmation : public CMessage
{
public:
	CMessageBuffer::eMessageType m_MessageType;
	Uint16 m_Counter;
	Uint8 m_ReturnValue;

	virtual bool setData(const CBinBuffer &b)
	{
		//CBinBuffer bb = b;                           // const!
		unsigned int pos = 0;
		m_MessageType = (CMessageBuffer::eMessageType)b.getUint8(pos);
		m_Counter = b.getUint16(pos);
		m_ReturnValue = b.getUint8(pos);
		return true;
	}
	
	virtual CBinBuffer &getData(CBinBuffer &b) const
	{
		b += (Uint8)m_MessageType;
		b += m_Counter;
		b += m_ReturnValue;
		return b;
	}

	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::confirmation;}
};

#endif

