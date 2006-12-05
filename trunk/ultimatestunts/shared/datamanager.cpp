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

CDataObject *CDataManager::getObject(CDataObject::eDataType type, unsigned int ID)
{
	if(ID >= (m_Objects[type]).size())
	{
		CString name;
		if(type == CDataObject::eNone)           name = "None";
		if(type == CDataObject::eTrack)          name = "Track";
		if(type == CDataObject::eCollisionModel) name = "CollisionModel";
		if(type == CDataObject::eBound)          name = "Bound";
		if(type == CDataObject::eTileModel)      name = "TileModel";
		if(type == CDataObject::eMaterial)       name = "Material";
		if(type == CDataObject::eMovingObject)   name = "MovingObject";
		if(type == CDataObject::eGraphObj)       name = "GraphObj";
		if(type == CDataObject::eTexture)        name = "Texture";
		if(type == CDataObject::eSample)         name = "Sample";
		printf("Errror in CDataManager::getObject (shared/datamanager.cpp):\n"
			"  Object requested with ID %d, but there are only %d"
			"  objects of type %d (%s)\n",
			ID, (m_Objects[type]).size(), type, name.c_str());
		return NULL;
	}

	return m_Objects[type][ID];
}
const CDataObject *CDataManager::getObject(CDataObject::eDataType type, unsigned int ID) const
{
	if(ID >= (m_Objects[type]).size())
	{
		CString name;
		if(type == CDataObject::eNone)           name = "None";
		if(type == CDataObject::eTrack)          name = "Track";
		if(type == CDataObject::eCollisionModel) name = "CollisionModel";
		if(type == CDataObject::eBound)          name = "Bound";
		if(type == CDataObject::eTileModel)      name = "TileModel";
		if(type == CDataObject::eMaterial)       name = "Material";
		if(type == CDataObject::eMovingObject)   name = "MovingObject";
		if(type == CDataObject::eGraphObj)       name = "GraphObj";
		if(type == CDataObject::eTexture)        name = "Texture";
		if(type == CDataObject::eSample)         name = "Sample";
		printf("Errror in CDataManager::getObject (shared/datamanager.cpp):\n"
			"  Object requested with ID %d, but there are only %d"
			"  objects of type %d (%s)\n",
			ID, (m_Objects[type]).size(), type, name.c_str());
		return NULL;
	}

	return m_Objects[type][ID];
}

int CDataManager::loadObject(const CString &filename, const CParamList plist, CDataObject::eDataType type)
{
	int ID = findObject(filename, plist, type);
	if(ID >= 0)
		return ID;

	//else

	CDataObject *obj = createObject(filename, plist, type);
	if(obj == NULL) return -1;
	if(!obj->load(filename, plist))
	{
		obj->unload();
		delete obj;
		return -1;
	}

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

	//Type-specific unload
	while(m_Objects[type].size() > 0)
		unloadObject(type, 0);
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

vector<CDataObject *> CDataManager::getSubset(CDataObject::eDataType type, const CString &subset)
{
	//printf("Indices: \"%s\"\n", subset.c_str());
	vector<CDataObject *> ret;

	CString indices = subset;
	while(true)
	{
		int sp = indices.inStr(' ');
		if(sp > 0)
		{
			int n = indices.mid(0,sp).toInt();
			indices= indices.mid(sp+1, indices.length()-sp-1);
			//printf("    Adding %d\n", n);
			CDataObject *obj = getObject(type, n);
			ret.push_back(obj);
		}
		else
		{
			//printf("    Adding %d\n", indices.toInt());
			CDataObject *obj = getObject(type, indices.toInt()); //the last index
			ret.push_back(obj);
			break;
		}
	}

	return ret;
}

CDataObject *CDataManager::createObject(const CString &filename, const CParamList &plist, CDataObject::eDataType type)
{
	if(type == CDataObject::eNone)
		return new CDataObject(this, CDataObject::eNone);

	return NULL; //to be overloaded
}

int CDataManager::findObject(const CString &filename, const CParamList plist, CDataObject::eDataType type)
{
	for(unsigned int i=0; i < m_Objects[type].size(); i++)
	{
		CDataObject *obj = m_Objects[type][i];
		if(obj->getFilename() == filename && obj->getParamList() == plist)
			return i;
	}

	return -1;
}

void CDataManager::unloadObject(unsigned int type, unsigned int i)
{
	CDataObject *obj = m_Objects[type][i];
	obj->unload();
	delete obj;
	m_Objects[type].erase(m_Objects[type].begin() + i);
}
