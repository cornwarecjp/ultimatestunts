/***************************************************************************
                          dataobject.cpp  -  Object loaded from a data file
                             -------------------
    begin                : wo dec 1 2004
    copyright            : (C) 2004 by CJP
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

#include "dataobject.h"

CDataObject::CDataObject(CDataManager *manager, eDataType type)
{
	m_DataManager = manager;
	m_DataType = type;
}

CDataObject::~CDataObject(){
}

bool CDataObject::load(const CString &idstring)
{
	if(m_isLoaded) unload();
	m_isLoaded = true;

	m_Filename = idstring;
	m_IDString = idstring;
	return true;
}

void CDataObject::unload()
{
	m_isLoaded = false;
}
