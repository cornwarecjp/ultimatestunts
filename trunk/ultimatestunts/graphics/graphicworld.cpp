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

#include <GL/gl.h>

#include "bound.h"
#include "graphicworld.h"
#include "datafile.h"
#include "world.h"


CGraphicMovObj::CGraphicMovObj(CGraphicWorld *world) : CDataObject(world, CDataObject::eMovingObject)
{
	m_Shadow = NULL;
	m_Dashboard = NULL;
}

bool CGraphicMovObj::load(const CString &filename, const CParamList &list)
{
	CDataObject::load(filename, list);

	unsigned int refln = m_ParamList.getValue("reflnum" , "1"   ).toInt();
	unsigned int refls = m_ParamList.getValue("reflsize", "1024").toInt();
	unsigned int shads = m_ParamList.getValue("shadsize", "1024").toInt();
	unsigned int ID    = m_ParamList.getValue("ID"      , "0"   ).toInt();

	for(unsigned int i=0; i < refln; i++)
	{
		//fprintf(stderr, "Reflection added: cam %d\n", i);
		m_Reflections.push_back(CDynamicReflection(refls));
	}

	if(shads > 4)
		m_Shadow = new CDynamicShadow(shads, (CGraphicWorld *)m_DataManager, ID);

	m_Dashboard = new CDashboard((CGraphicWorld *)m_DataManager, ID);

	return true;
}

void CGraphicMovObj::unload()
{
	if(!isLoaded()) return;

	CDataObject::unload();

	m_Reflections.clear();

	if(m_Shadow != NULL) delete m_Shadow;
	if(m_Dashboard != NULL) delete m_Dashboard;
}





CGraphicWorld::CGraphicWorld()
{
	m_World = theWorld;
	m_TrackMapDisplayList = 0;

	reloadConfiguration();

	//Create objects:
	m_Background = new CBackground(this);
	m_EnvMap = new CStaticReflection(this);
}

bool CGraphicWorld::reloadConfiguration()
{
	//Defaults:
	m_TexMaxSize = m_BackgroundSize = m_ReflectionSize = m_ShadowSize = 1024;

	CString cnf = theMainConfig->getValue("graphics", "texture_maxsize");
	if(cnf != "")
		m_TexMaxSize = cnf.toInt();

	cnf = theMainConfig->getValue("graphics", "background_size");
	if(cnf != "")
		m_BackgroundSize = cnf.toInt();

	cnf = theMainConfig->getValue("graphics", "reflection_size");
	if(cnf != "")
		m_ReflectionSize = cnf.toInt();
	
	cnf = theMainConfig->getValue("graphics", "shadow_size");
	if(cnf != "")
		m_ShadowSize = cnf.toInt();

	cnf = theMainConfig->getValue("graphics", "texture_smooth");
	m_TexSmooth = (cnf != "false");

	cnf = theMainConfig->getValue("graphics", "draw_map");
	m_DrawMap = (cnf != "false");

	//Unload previous lens flare
	for(unsigned int i=0; i < m_LensFlare.size(); i++)
		delete m_LensFlare[i].image;
	m_LensFlare.clear();

	//Load lens flare
	if(theMainConfig->getValue("graphics", "lensflare_enable") == "true")
	{
		CString flare_files     = theMainConfig->getValue("graphics", "lensflare_files");
		CString flare_sizes     = theMainConfig->getValue("graphics", "lensflare_sizes");
		CString flare_distances = theMainConfig->getValue("graphics", "lensflare_distances");

		while(true)
		{
			int comma_files = flare_files.inStr(',');
			int comma_sizes = flare_sizes.inStr(',');
			int comma_distances = flare_distances.inStr(',');

			CString file;
			float size, distance;

			if(comma_files < 0)
				{file = flare_files;}
			else
			{
				file = flare_files.mid(0, comma_files);
				flare_files = flare_files.mid(comma_files+1);
			}

			if(comma_sizes < 0)
				{size = flare_sizes.toFloat();}
			else
			{
				size = flare_sizes.mid(0, comma_sizes).toFloat();
				flare_sizes = flare_sizes.mid(comma_sizes+1);
			}

			if(comma_distances < 0)
				{distance = flare_distances.toFloat();}
			else
			{
				distance = flare_distances.mid(0, comma_distances).toFloat();
				flare_distances = flare_distances.mid(comma_distances+1);
			}

			printf("Loading lensflare distance %.3f size %.3f file %s\n",
				distance, size, file.c_str());

			SLensFlare flare;
			flare.distance = distance;
			flare.size = size;
			flare.image = new CTexture(this);
			flare.image->load(file, CParamList());
			m_LensFlare.push_back(flare);

			if(comma_files < 0 || comma_sizes < 0 || comma_distances < 0) break;
		}
	}

	return true;
}

CGraphicWorld::~CGraphicWorld()
{
	unloadWorld();
	delete m_Background;
	delete m_EnvMap;

	for(unsigned int i=0; i < m_LensFlare.size(); i++)
		delete m_LensFlare[i].image;
	m_LensFlare.clear();
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

	if(type == CDataObject::eMovingObject)
		return new CGraphicMovObj(this);

	return NULL;
}

bool CGraphicWorld::loadWorld()
{
	printf("Loading the graphic world\n");

	printf("  Loading tile textures:\n");
	//First unload all currently loaded textures to get the IDs right
	unloadAll(CDataObject::eMaterial);
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
	//First unload all currently loaded tiles to get the IDs right
	unloadAll(CDataObject::eTileModel);
	for(unsigned int i=0; i<m_World->getNumObjects(CDataObject::eTileModel); i++)
		loadObject(m_World->getTileModel(i)->getFilename(), m_World->getTileModel(i)->getParamList(), CDataObject::eTileModel);


	printf("  Loading background and environment map:\n");
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

	m_Background->load(m_World->getTrack()->m_BackgroundFilename, plist);
	m_EnvMap->load(m_World->getTrack()->m_EnvMapFilename, plist);

	printf("  Loading track map\n");
	loadTrackMap();

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
	//First unload all currently loaded textures to get the IDs right
	unloadAll(CDataObject::eBound);
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

	//Allocate room for reflection and shadow textures
	for(unsigned int i=0; i<m_World->getNumObjects(CDataObject::eMovingObject); i++)
	{
		CParamList plist;
		SParameter p;
		p.name = "ID";
		p.value = i;
		plist.push_back(p);
		p.name = "reflnum";
		p.value = m_World->getNumObjects(CDataObject::eMovingObject);
		plist.push_back(p);
		p.name = "reflsize";
		p.value = m_ReflectionSize;
		plist.push_back(p);
		p.name = "shadsize";
		p.value = m_ShadowSize;
		plist.push_back(p);

		loadObject(m_World->getMovingObject(i)->getFilename(), plist, CDataObject::eMovingObject);
	}

	return true;
}

void CGraphicWorld::loadTrackMap()
{
	if(m_TrackMapDisplayList == 0)
		m_TrackMapDisplayList = glGenLists(1);

	glNewList(m_TrackMapDisplayList, GL_COMPILE);

	const CTrack *track = theWorld->getTrack();
	if(track != NULL)
	{
		float size = (track->m_L > track->m_W) ? track->m_L : track->m_W;
		glScalef(1.0 / size, 1.0 / size, 1.0);

		for(unsigned int i=0; i < track->m_Routes.size(); i++)
		{
			const CTrack::CRoute &route = track->m_Routes[i];
			if(route.size() < 2) continue;

			glBegin(GL_LINE_STRIP);

			for(unsigned int j=1; j < route.size(); j++)
			{
				const CTrack::CCheckpoint &c1 = route[j-1];
				const CTrack::CCheckpoint &c2 = route[j  ];

				glVertex2f(c1.x, c1.z);
				glVertex2f(c2.x, c2.z);
			}

			glEnd();
		}
	}

	glEndList();
}

void CGraphicWorld::drawTrackMap()
{
	glCallList(m_TrackMapDisplayList);
}
