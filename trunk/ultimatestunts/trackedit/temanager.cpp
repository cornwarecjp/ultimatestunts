/***************************************************************************
                          temanager.cpp  -  The data manager for the track editor
                             -------------------
    begin                : ma mei 23 2005
    copyright            : (C) 2005 by CJP
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

#include "temanager.h"

#include "edittrack.h"
#include "lodtexture.h"
#include "graphobj.h"

CTEManager::CTEManager(){
}
CTEManager::~CTEManager(){
}

CDataObject *CTEManager::createObject(const CString &filename, const CParamList &plist, CDataObject::eDataType type)
{
	CDataObject *obj = CDataManager::createObject(filename, plist, type);
	if(obj != NULL) return obj;

	if(type == CDataObject::eMaterial)
		return new CLODTexture(this);

	if(type == CDataObject::eTileModel)
		return new CGraphObj(this, CDataObject::eTileModel);

	if(type == CDataObject::eBound)
		return new CGraphObj(this, CDataObject::eBound);

	if(type == CDataObject::eTrack)
		return new CEditTrack(this);

	return NULL;
}
