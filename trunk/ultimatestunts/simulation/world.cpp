/***************************************************************************
                          world.cpp  -  The world: tiles and cars and other objects
                             -------------------
    begin                : Wed Dec 4 2002
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

#include "world.h"
#include "car.h"
#include "datafile.h"
#include "bound.h"

CWorld *theWorld = NULL;

CWorld::CWorld()
{
	printDebug = false;
	m_Paused = true;

	theWorld = this;
}

CWorld::~CWorld(){
	unloadAll();

	//dJointGroupDestroy(m_ContactGroup);
	//dWorldDestroy(m_ODEWorld);

	theWorld = NULL;
}

CDataObject *CWorld::createObject(const CString &filename, const CParamList &plist, CDataObject::eDataType type)
{
	CDataObject *obj = CDataManager::createObject(filename, plist, type);
	if(obj != NULL) return obj;

	if(type == CDataObject::eSample)
		return new CDataObject(this, CDataObject::eSample);

	if(type == CDataObject::eMaterial)
		return new CMaterial(this);

	if(type == CDataObject::eBound)
		return new CBound(this);

	if(type == CDataObject::eTileModel)
		return new CTileModel(this);

	if(type == CDataObject::eMovingObject)
		//TODO: other moving object types
		return new CCar(this);

	if(type == CDataObject::eTrack)
		return new CTrack(this);

	return NULL;
}
