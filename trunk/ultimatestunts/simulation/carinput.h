/***************************************************************************
                          carinput.h  -  description
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

#include <SDL/SDL.h>
#include "usmacros.h"

#include "movobjinput.h"

/**
  *@author CJP
  */

class CCarInput : public CMovObjInput  {
public: 
	//Which methods should be re-implemented?

	Uint8 m_Gear;
	bool m_CarHorn;

	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::carInput;}
};

#endif
