/***************************************************************************
                          graphicworld.cpp  -  Graphical world data
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
#include <cstdio>

#include "bound.h"
#include "graphicworld.h"
#include "datafile.h"
#include "world.h"

CGraphicWorld::CGraphicWorld()
{
	m_World = theWorld;

	//Defaults:
	m_TexMaxSize = m_BackgroundSize = 1024;

	CString cnf = theMainConfig->getValue("graphics", "texture_maxsize");
	if(cnf != "")
		m_TexMaxSize = cnf.toInt();

	cnf = theMainConfig->getValue("graphics", "background_size");
	if(cnf != "")
		m_BackgroundSize = cnf.toInt();

	cnf = theMainConfig->getValue("graphics", "texture_smooth");
	m_TexSmooth = (cnf != "false");

	//Create objects:
	m_Background = new CBackground(this);
	m_EnvMap = new CStaticReflection(m_TexSmooth, this);
}

CGraphicWorld::~CGraphicWorld()
{
	unloadWorld();
	delete m_Background;
	delete m_EnvMap;
}

CDataObject *CGraphicWorld::createObject(const CString &filename, const CParamList &plist, CDataObject::eDataType type)
{
	CDataObject *obj = CDataManager::createObject(filename, plist, type);
	if(obj != NULL) return obj;

	if(type == CDataObject::eMaterial)
		return new CLODTexture(this);

	if(type == CDataObject::eTileModel)
		return new CGraphObj(this, CDataObject::eTileModel);

	if(type == CDataObject::eBound)
		return new CGraphObj(this, CDataObject::eBound);

	return NULL;
}

bool CGraphicWorld::loadWorld()
{
	printf("Loading the graphic world\n");

	printf("  Loading tile textures:\n");
	for(unsigned int i=0; i<m_World->getNumObjects(CDataObject::eMaterial); i++)
	{
		int mul = m_World->getMaterial(i)->m_Mul;
		int xs = m_TexMaxSize / mul;
		int ys = m_TexMaxSize / mul;
		CParamList plist;
		SParameter p;
		p.name = "sizex";
		p.value = xs;
		plist.push_back(p);
		p.name = "sizey";
		p.value = ys;
		plist.push_back(p);
		p.name = "smooth";
		p.value = m_TexSmooth;
		plist.push_back(p);

		//TODO: check ID
		loadObject(m_World->getMaterial(i)->getFilename(), plist, CDataObject::eMaterial);
	}

	printf("  Loading tiles:\n");
	for(unsigned int i=0; i<m_World->getNumObjects(CDataObject::eTileModel); i++)
		loadObject(m_World->getTileModel(i)->getFilename(), m_World->getTileModel(i)->getParamList(), CDataObject::eTileModel);

	//params for background and envmap
	CParamList plist;
	SParameter p;
	p.name = "sizex";
	p.value = m_BackgroundSize;
	plist.push_back(p);
	p.name = "sizey";
	p.value = m_BackgroundSize;
	plist.push_back(p);
	p.name = "smooth";
	p.value = m_TexSmooth;
	plist.push_back(p);

	//printf("  Loading background %s:\n", fb.getName().c_str());
	m_Background->load(m_World->getTrack()->m_BackgroundFilename, plist);

	//printf("  Loading environment map %s:\n", fe.getName().c_str());
	m_EnvMap->load(m_World->getTrack()->m_EnvMapFilename, plist);

	return true;
}

void CGraphicWorld::unloadWorld()
{
	printf("Unloading the graphic world\n");

	printf("  Unloading background\n");
	m_Background->unload();

	printf("  Unloading environment map\n");
	m_EnvMap->unload();

	unloadAll();
}

bool CGraphicWorld::loadObjects()
{
	int molod = theMainConfig->getValue("graphics", "movingobjectlod").toInt();

	printf("Loading moving object graphics:\n");

	//Load the rest of the textures
	for(unsigned int i=0; i<m_World->getNumObjects(CDataObject::eMaterial); i++)
	{
		int mul = m_World->getMaterial(i)->m_Mul;
		int xs = m_TexMaxSize / mul;
		int ys = m_TexMaxSize / mul;
		CParamList plist;
		SParameter p;
		p.name = "sizex";
		p.value = xs;
		plist.push_back(p);
		p.name = "sizey";
		p.value = ys;
		plist.push_back(p);
		p.name = "smooth";
		p.value = m_TexSmooth;
		plist.push_back(p);

		//TODO: check ID
		loadObject(m_World->getMaterial(i)->getFilename(), plist, CDataObject::eMaterial);
	}

	//Body graphics
	vector<const CDataObject *> bounds = m_World->getObjectArray(CDataObject::eBound);
	for(unsigned int i=0; i<bounds.size(); i++)
	{
		CParamList plist = bounds[i]->getParamList(); //parameters like "subset"
		SParameter p;
		p.name = "lodoffset";
		p.value = molod;
		plist.push_back(p);

		loadObject(bounds[i]->getFilename(), plist, CDataObject::eBound);
	}

	return true;
}
