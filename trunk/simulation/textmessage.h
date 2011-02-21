/***************************************************************************
                          textmessage.h  -  A simple text message
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

#ifndef TEXTMESSAGE_H
#define TEXTMESSAGE_H

#include "message.h"
#include "cstring.h"

/**
  *@author CJP
  */

class CTextMessage : public CMessage  {
public: 
	CTextMessage();
	virtual ~CTextMessage();

	//For network transfer & other stuff
	virtual CBinBuffer &getData(CBinBuffer &b) const;            // returns class data as binbuffer
	virtual bool setData(const CBinBuffer &b, unsigned int &pos);// rebuild class data from binbuffer

	virtual CMessageBuffer::eMessageType getType() const
		{return CMessageBuffer::textMessage;}

	CString m_Message;
};

#endif
