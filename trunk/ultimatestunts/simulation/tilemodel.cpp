/***************************************************************************
                          tilemodel.cpp  -  The (collision) model of a tile
                             -------------------
    begin                : wo sep 24 2003
    copyright            : (C) 2003 by CJP
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

#include "tilemodel.h"

CTileModel::CTileModel(CDataManager *manager) : CCollisionModel(manager)
{
	m_DataType = CDataObject::eTileModel;
}

CTileModel::~CTileModel(){
}

bool CTileModel::load(const CString &idstring)
{
	CString line = idstring;
	CString tile_flags, texture_indices;;
	int pos = line.inStr(' ');
	if(pos  > 0) //a space exists
	{
		if((unsigned int)pos < line.length()-1)
			{texture_indices = line.mid(pos+1, line.length()-pos-1);}
		line = line.mid(0, pos);
	}
	pos = line.inStr(':');
	if(pos > 0) //a : exists
	{
		if((unsigned int)pos < line.length()-1)
			{tile_flags = line.mid(pos+1, line.length()-pos-1);}
		line = line.mid(0, pos);
	}

	line = line + " " + texture_indices;

	m_isStart = tile_flags.inStr('s') >= 0;
	m_isFinish = tile_flags.inStr('f') >= 0;
	m_Time = 1.0;

	bool ret = CCollisionModel::load(line);

	m_IDString = idstring;

	return ret;
}
