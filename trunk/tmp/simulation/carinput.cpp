/***************************************************************************
                          carinput.cpp  -  Input for car objects
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

#include "carinput.h"

CCarInput::CCarInput()
{
	m_Gear = 1;
	m_CarHorn = false;
}

CBinBuffer &CCarInput::getData(CBinBuffer & b) const
{
	CMovObjInput::getData(b);

	b += Uint8(m_Gear);
	b += Uint8(m_CarHorn);

	return b;
}

bool CCarInput::setData(const CBinBuffer & b, unsigned int &pos)
{
	if(!CMovObjInput::setData(b, pos)) return false;

	m_Gear = b.getUint8(pos);
	m_CarHorn = b.getUint8(pos);

	return true;
}
