/***************************************************************************
                          datamanager.cpp  -  Managing loading/unloading data objects
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

#include "datamanager.h"
#include "dataobject.h"

CDataManager::CDataManager()
{
	//initialise array
	for(unsigned int i=0; i < CDataObject::eEnumTop; i++)
		m_Objects.push_back(vector<CDataObject *>());
}

CDataManager::~CDataManager()
{
	unloadAll(CDataObject::eNone);
}

int CDataManager::getObjectID(const CString &idstring, CDataObject::eDataType type)
{
	int ID = findObject(idstring, type);
	if(ID >= 0)
		return ID;

	//else
	CDataObject *obj = createObject(idstring, type);
	if(obj == NULL) return -1;

	m_Objects[type].push_back(obj);
	return m_Objects[type].size() - 1;
}

void CDataManager::unloadAll(CDataObject::eDataType type)
{
	if(type == CDataObject::eNone)
	{
		for(unsigned int t=0; t < CDataObject::eEnumTop; t++)
			while(m_Objects[t].size() > 0)
				unloadObject(t, 0);

		return;
	}

	//TODO: type-specific unload
}

vector<CDataObject *> CDataManager::getObjectArray(CDataObject::eDataType type)
{
	return m_Objects[type];
}

vector<const CDataObject *> CDataManager::getObjectArray(CDataObject::eDataType type) const
{
	vector<const CDataObject *> ret;
	for(unsigned int i=0; i < m_Objects[type].size(); i++)
		ret.push_back(m_Objects[type][i]);

	return ret;
}

CDataObject *CDataManager::createObject(const CString &idstring, CDataObject::eDataType type)
{
	if(type == CDataObject::eNone)
	{
		CDataObject *obj = new CDataObject(this, CDataObject::eNone);
		if(obj == NULL) return NULL;
		if(!obj->load(idstring))
		{
			delete obj;
			return NULL;
		}
		return obj;
	}

	return NULL; //to be overloaded
}

int CDataManager::findObject(const CString &idstring, CDataObject::eDataType type)
{
	for(unsigned int i=0; i < m_Objects[type].size(); i++)
	{
		CDataObject *obj = m_Objects[type][i];
		if(obj->getIDString() == idstring)
			return i;
	}

	return -1;
}

void CDataManager::unloadObject(unsigned int type, unsigned int i)
{
	CDataObject *obj = m_Objects[type][i];
	delete obj;
	m_Objects[type].erase(m_Objects[type].begin() + i);
}
