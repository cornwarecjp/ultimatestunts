/***************************************************************************
                          movobjinput.cpp  -  Input for moving objects
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
#include "movobjinput.h"

CMovObjInput::CMovObjInput()
{
	m_Forward = m_Backward = m_Up = m_Right = 0.0;
}

CBinBuffer &CMovObjInput::getData(CBinBuffer & b) const
{
	b.addFloat8(m_Up, 1.0);
	b.addFloat8(m_Forward, 1.0);
	b.addFloat8(m_Backward, 1.0);
	b.addFloat8(m_Right, 1.0);
	return b;
}

bool CMovObjInput::setData(const CBinBuffer & b)
{
	unsigned int pos = 0;
	m_Up = b.getFloat8(pos, 1.0);
	m_Forward = b.getFloat8(pos, 1.0);
	m_Backward = b.getFloat8(pos, 1.0);
	m_Right = b.getFloat8(pos, 1.0);
	return true;
}
