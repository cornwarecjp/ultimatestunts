/***************************************************************************
                          message.h  -  description
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

#ifndef MESSAGE_H
#define MESSAGE_H


/**
  *@author CJP
  */

#include "messagebuffer.h"

class CMessage {
public: 
	CMessage();
  CMessage(CMessageBuffer b);
	virtual ~CMessage();

  virtual CMessageBuffer getBuffer();
  virtual void setBuffer(CMessageBuffer b);
};

#endif
