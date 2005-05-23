/***************************************************************************
                          graphicworld.h  -  Graphical world data
                             -------------------
    begin                : do jan 16 2003
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

#ifndef GRAPHICWORLD_H
#define GRAPHICWORLD_H

#include "lodtexture.h"
#include "graphobj.h"
#include "background.h"
#include "world.h"
#include "lconfig.h"
#include "staticreflection.h"
#include "datamanager.h"

/**
  *@author CJP
  */

class CGraphicWorld : public CDataManager
{
public: 
	CGraphicWorld();
	virtual ~CGraphicWorld();

	bool loadWorld();
	void unloadWorld();
	bool loadObjects();

	CGraphObj *getTile(unsigned int n)
		{return (CGraphObj *)getObject(CDataObject::eTileModel, n);}
	CGraphObj *getMovObjBound(unsigned int n)
		{return (CGraphObj *)getObject(CDataObject::eBound, n);}

	CBackground *m_Background;
	CStaticReflection *m_EnvMap;
protected:
	virtual CDataObject *createObject(const CString &filename, const CParamList &plist, CDataObject::eDataType type);

	const CWorld *m_World;
	int m_TexMaxSize;
	int m_BackgroundSize;
	bool m_TexSmooth;
};

#endif
