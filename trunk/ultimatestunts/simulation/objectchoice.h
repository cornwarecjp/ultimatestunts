/***************************************************************************
                          objectchoice.h  -  description
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

#include <SDL/SDL.h>
#include "message.h"

/**
  *@author bones
  */

class CObjectChoice : public CMessage  {
public: 
  CObjectChoice();
  ~CObjectChoice();

  bool setData(const CBinBuffer &);
  CBinBuffer & getData() const;

  Uint16 m_CarNumber; //temporary; probably replaced by CString

	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::objectChoice;}
};

#endif
