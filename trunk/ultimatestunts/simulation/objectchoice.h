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

#include "message.h"

/**
  *@author CJP
  */

class CObjectChoice : public CMessage  {
public: 
	CObjectChoice();
  CObjectChoice(CMessageBuffer b);
	virtual ~CObjectChoice();

  virtual CMessageBuffer getBuffer();
  virtual void setBuffer(CMessageBuffer b);

  int m_CarNumber; //temporary; probably replaced by CString
};

#endif
