/***************************************************************************
                          movobjinput.h  -  Input for moving objects
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

#ifndef MOVOBJINPUT_H
#define MOVOBJINPUT_H

#include "message.h"

/**
  *@author CJP
  */

class CMovObjInput : public CMessage  {
public:
	//Which CMessage-methods should be re-implemented?
	CMovObjInput(); //Give reasonable start values

	float m_Up, m_Forward, m_Backward, m_Right;

	bool setData(const CBinBuffer &);
	CBinBuffer &getData(CBinBuffer &) const;
	Uint8 m_MovObjID;

	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::movObjInput;}
};

#endif
