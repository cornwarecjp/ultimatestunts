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

#include <cstdio>

#include "dataobject.h"

bool CParamList::operator==(const CParamList &val) const
{
	for(unsigned int i=0; i < val.size(); i++)
	{
		CString name = val[i].name;
		CString value = val[i].value;

		CString ownval = getValue(name, "__NoTaVaLuE__");
		if(ownval != value) return false;
	}
	
	for(unsigned int i=0; i < size(); i++)
	{
		CString name = (*this)[i].name;
		CString ownval = (*this)[i].value;

		CString value = val.getValue(name, "__NoTaVaLuE__");
		if(ownval != value) return false;
	}

	return true;
}

CString CParamList::getValue(const CString &var, const CString &deflt) const
{
	for(unsigned int i=0; i < size(); i++)
		if ((*this)[i].name == var)
			return (*this)[i].value;

	return deflt;
}

CDataObject::CDataObject(CDataManager *manager, eDataType type)
{
	m_DataManager = manager;
	m_DataType = type;
}

CDataObject::~CDataObject(){
}

bool CDataObject::load(const CString &filename, const CParamList &list)
{
	printf("   Loading file %s\n", filename.c_str());
	//for(unsigned int i=0; i < list.size(); i++)
	//	printf("     %s = \"%s\"\n", list[i].name.c_str(), list[i].value.c_str());

	if(m_isLoaded) unload();
	m_isLoaded = true;

	m_Filename = filename;
	m_ParamList = list;
	return true;
}

void CDataObject::unload()
{
	m_isLoaded = false;
}
