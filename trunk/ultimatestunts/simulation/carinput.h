/***************************************************************************
                          carinput.h  -  Input for car objects
                             -------------------
    begin                : ma dec 16 2002
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

#ifndef CARINPUT_H
#define CARINPUT_H

#include "usmacros.h"

#include "movobjinput.h"

/**
  *@author CJP
  */

class CCarInput : public CMovObjInput  {
public: 
	//Which methods should be re-implemented?
	CCarInput(); //Give reasonable start values

	Uint8 m_Gear;
	bool m_CarHorn;

	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::carInput;}
};

#endif
