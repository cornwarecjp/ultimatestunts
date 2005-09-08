/***************************************************************************
                          objectchoice.h  -  Describes which moving object is chosen
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

#ifndef OBJECTCHOICE_H
#define OBJECTCHOICE_H

#include "SDL.h"
#include "message.h"
#include "cstring.h"

#define OBJECTCHOICE_REFUSED 255 //network return value for refusing a new object

/**
  *@author bones
  */

class CObjectChoice : public CMessage  {
public: 
	CObjectChoice();
	~CObjectChoice();

	bool setData(const CBinBuffer &, unsigned int &pos);
	CBinBuffer &getData(CBinBuffer &) const;

	CMessageBuffer::eMessageType m_ObjType;
	CString m_Filename;
	//Additional: (car) settings
	//Additional: selected skin / color

	CString m_PlayerName;

	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::objectChoice;}
};

#endif
