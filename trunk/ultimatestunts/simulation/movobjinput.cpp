/***************************************************************************
                          movobjinput.cpp  -  description
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

CBinBuffer & CMovObjInput::getData() const
{
	CBinBuffer *res = new CBinBuffer();
	//(*res)+=m_Up; //+=float not yet implemented
	//(*res)+=m_Forward;
	//(*res)+=m_Backward;
	//(*res)+=m_Right;
	return (*res);
}

bool CMovObjInput::setData(const CBinBuffer & b)
{
	CBinBuffer bb = b;                           // const!
	//m_Up = bb.getFloat(0); //getFloat not yet implemented
	//m_Forward = bb.getFloat();
	//m_Backward = bb.getFloat();
	//m_Right = bb.getFloat();
	return (true);
}
