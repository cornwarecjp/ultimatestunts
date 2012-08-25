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

#include <cstdio>

#include "movobjinput.h"

CMovObjInput::CMovObjInput()
{
	m_Forward = m_Backward = m_Up = m_Right = 0.0;
}

CBinBuffer &CMovObjInput::getData(CBinBuffer & b) const
{
	b += m_MovObjID;
	b.addFloat8(m_Up, 0.008);
	b.addFloat8(m_Forward, 0.008);
	b.addFloat8(m_Backward, 0.008);
	b.addFloat8(m_Right, 0.008);
	return b;
}

bool CMovObjInput::setData(const CBinBuffer & b, unsigned int &pos)
{
	Uint8 ID = b.getUint8(pos);
	if(m_MovObjID != ID)
	{
		printf("Error: object ID = %d, message ID = %d\n", m_MovObjID, ID);
		return false;
	}

	m_Up = b.getFloat8(pos, 0.008);
	m_Forward = b.getFloat8(pos, 0.008);
	m_Backward = b.getFloat8(pos, 0.008);
	m_Right = b.getFloat8(pos, 0.008);

	return true;
}
