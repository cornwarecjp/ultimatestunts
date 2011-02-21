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

bool CTileModel::load(const CString &filename, const CParamList &list)
{
	CCollisionModel::load(filename, list);

	CString tile_flags = m_ParamList.getValue("flags", "");

	m_isStart = tile_flags.inStr('s') >= 0;
	m_isFinish = tile_flags.inStr('f') >= 0;
	m_Time = 1.0;

	return true;
}
