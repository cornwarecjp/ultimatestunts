/***************************************************************************
                          objectchoice.cpp  -  Describes which moving object is chosen
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


#include "binbuffer.h"
#include "objectchoice.h"

CObjectChoice::CObjectChoice()
{
	m_ObjType = CMessageBuffer::movingObject;
	m_Filename = "not_a_file.conf";
	m_PlayerName = "Someone";
}

CObjectChoice::~CObjectChoice()
{}


CBinBuffer &CObjectChoice::getData(CBinBuffer &b) const
{
	b += (Uint8)m_ObjType;
	b += m_Filename;
	b += m_PlayerName;
	b += (Uint8)(m_Parameters.size());
	for(unsigned int i=0; i < m_Parameters.size(); i++)
	{
		b += m_Parameters[i].name;
		b += m_Parameters[i].value;
	}

	return b;
}

bool CObjectChoice::setData(const CBinBuffer & b, unsigned int &pos)
{
	m_ObjType = (CMessageBuffer::eMessageType)b.getUint8(pos);
	m_Filename = b.getCString(pos);
	m_PlayerName = b.getCString(pos);
	unsigned int size = b.getUint8(pos);
	m_Parameters.resize(size);
	for(unsigned int i=0; i < size; i++)
	{
		m_Parameters[i].name = b.getCString(pos);
		m_Parameters[i].value = b.getCString(pos);
	}

	return true;
}

