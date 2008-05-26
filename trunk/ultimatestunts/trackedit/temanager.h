/***************************************************************************
                          temanager.h  -  The data manager for the track editor
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

#ifndef TEMANAGER_H
#define TEMANAGER_H

#include "datamanager.h"
#include "edittrack.h"
#include "tetile.h"

/**
  *@author CJP
  */

class CTEManager : public CDataManager  {
public: 
	CTEManager();
	~CTEManager();

	const CEditTrack *getTrack() const
		{return (const CEditTrack *)getObject(CDataObject::eTrack, 0);}
	CEditTrack *getTrack()
		{return (CEditTrack *)getObject(CDataObject::eTrack, 0);}
	CTETile *getTile(unsigned int ID)
		{return (CTETile *)getObject(CDataObject::eTileModel, ID);}
	const CTETile *getTile(unsigned int ID) const
		{return (CTETile *)getObject(CDataObject::eTileModel, ID);}

	void removeUnusedTextures();

	//This function loads new textures if necessary:
	CString getTextureSubset(const CString &textureString);

protected:
	virtual CDataObject *createObject(const CString &filename, const CParamList &plist, CDataObject::eDataType type);
};

#endif
