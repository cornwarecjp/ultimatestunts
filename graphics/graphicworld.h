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

class CGraphicWorld;

#include "lconfig.h"

#include "lodtexture.h"
#include "graphobj.h"

#include "staticreflection.h"
#include "background.h"
#include "world.h"

#include "dashboard.h"
#include "dynamicreflection.h"
#include "dynamicshadow.h"
#include "smoke.h"

#include "datamanager.h"

/**
  *@author CJP
  */

class CGraphicMovObj : public CDataObject
{
public:
	CGraphicMovObj(CGraphicWorld *world);

	virtual bool load(const CString &filename, const CParamList &list);
	virtual void unload();

	vector<CDynamicReflection> m_Reflections;
	CDynamicShadow *m_Shadow;
	CDashboard *m_Dashboard;

	CSmoke m_CrashSmoke;
};

class CGraphicWorld : public CDataManager
{
public: 
	CGraphicWorld();
	virtual ~CGraphicWorld();

	virtual bool reloadConfiguration();

	bool loadWorld();
	void unloadWorld();
	bool loadObjects();

	CGraphObj *getTile(unsigned int n)
		{return (CGraphObj *)getObject(CDataObject::eTileModel, n);}

	CGraphObj *getMovObjBound(unsigned int n)
		{return (CGraphObj *)getObject(CDataObject::eBound, n);}

	const CGraphObj *getMovObjBound(unsigned int n) const
		{return (const CGraphObj *)getObject(CDataObject::eBound, n);}

	CGraphicMovObj *getMovObj(unsigned int n)
		{return (CGraphicMovObj *)getObject(CDataObject::eMovingObject, n);}
	CDynamicReflection *getMovObjReflection(unsigned int n, unsigned int cam)
		{return &(getMovObj(n)->m_Reflections[cam]);}
	CDynamicShadow *getMovObjShadow(unsigned int n)
		{return getMovObj(n)->m_Shadow;}
	CDashboard *getMovObjDashboard(unsigned int n)
		{return getMovObj(n)->m_Dashboard;}

	void drawTrackMap();

	CBackground *m_Background;
	CStaticReflection *m_EnvMap;

	struct SLensFlare
	{
		CTexture *image;
		float size;
		float distance;
	};
	vector<SLensFlare> m_LensFlare;

	//Some settings
	int m_TexMaxSize;
	int m_BackgroundSize;
	int m_ReflectionSize;
	int m_ShadowSize;
	bool m_TexSmooth;
	bool m_DrawMap;

protected:
	virtual CDataObject *createObject(const CString &filename, const CParamList &plist, CDataObject::eDataType type);

	const CWorld *m_World;

	void loadTrackMap();
	unsigned int m_TrackMapDisplayList;
};

#endif
