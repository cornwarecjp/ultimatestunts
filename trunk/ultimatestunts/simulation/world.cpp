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

	m_ODEWorld = dWorldCreate();

	/*
	I know that this defies the law of gravity, but,
	you see, I never studied law.

	Bugs Bunny
	*/
	dWorldSetGravity(m_ODEWorld, 0.0, -9.81, 0.0);
	m_ContactGroup = dJointGroupCreate(0);

	theWorld = this;
}

CWorld::~CWorld(){
	unloadAll();

	dJointGroupDestroy(m_ContactGroup);
	dWorldDestroy(m_ODEWorld);

	theWorld = NULL;
}

CDataObject *CWorld::createObject(const CString &idstring, CDataObject::eDataType type)
{
	CDataObject *obj = CDataManager::createObject(idstring, type);
	if(obj != NULL) return obj;

	if(type == CDataObject::eSample)
	{
		obj = new CDataObject(this, CDataObject::eSample);
		if(obj == NULL) return NULL;
		if(!obj->load(idstring))
		{
			delete obj;
			return NULL;
		}
		return obj;
	}

	if(type == CDataObject::eMaterial)
	{
		obj = new CMaterial(this);
		if(obj == NULL) return NULL;
		if(!obj->load(idstring))
		{
			delete obj;
			return NULL;
		}
		return obj;
	}

	if(type == CDataObject::eBound)
	{
		obj = new CBound(this);
		if(obj == NULL) return NULL;
		if(!obj->load(idstring))
		{
			delete obj;
			return NULL;
		}

		//a little dirty hack
		if(idstring.inStr("wheel") >= 0)
			((CBound *)obj)->setCylinder(true);

		return obj;
	}

	if(type == CDataObject::eTileModel)
	{
		obj = new CTileModel(this);
		if(obj == NULL) return NULL;
		if(!obj->load(idstring))
		{
			delete obj;
			return NULL;
		}
		return obj;
	}

	if(type == CDataObject::eMovingObject)
	{
		//TODO: other moving object types
		obj = new CCar(this);
		if(obj == NULL) return NULL;
		if(!obj->load(idstring))
		{
			delete obj;
			return NULL;
		}
		return obj;
	}

	if(type == CDataObject::eTrack)
	{
		//TODO: other moving object types
		obj = new CTrack(this);
		if(obj == NULL) return NULL;
		if(!obj->load(idstring))
		{
			delete obj;
			return NULL;
		}
		return obj;
	}

	return NULL;
}
