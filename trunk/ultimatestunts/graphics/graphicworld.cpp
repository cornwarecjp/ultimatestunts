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
#include <stdio.h>

#include "graphicworld.h"
#include "datafile.h"

CGraphicWorld::CGraphicWorld(const CWorld *world, const CLConfig &conf)
{
	m_World = world;

	//Defaults:
	m_TexMaxSize = m_BackgroundSize = 1024;

	CString cnf = conf.getValue("graphics", "texture_maxsize");
	if(cnf != "")
		m_TexMaxSize = cnf.toInt();

	cnf = conf.getValue("graphics", "background_size");
	if(cnf != "")
		m_BackgroundSize = cnf.toInt();

	cnf = conf.getValue("graphics", "texture_smooth");
	m_TexSmooth = (cnf != "false");
}

CGraphicWorld::~CGraphicWorld()
{
	unloadWorld();
	unloadObjects();
}

bool CGraphicWorld::loadWorld()
{
	printf("Loading the graphic world\n");

	printf("Loading tile textures:\n");
	for(unsigned int i=0; i<m_World->m_TileMaterials.size(); i++)
	{
		CTexture t;
		CDataFile f(m_World->m_TileMaterials[i]->getFilename());
		int mul = m_World->m_TileMaterials[i]->getTextureMul();
		printf("loading %s with mul=%d:\n", f.getName().c_str(), mul);
		t.setTextureSmooth(m_TexSmooth);
		int xs = m_TexMaxSize / mul;
		int ys = m_TexMaxSize / mul;
		t.loadFromFile(f.useExtern(), xs, ys);
		m_TileTextures.push_back(t);
	}

	printf("Loading tiles:\n");
	for(unsigned int i=0; i<m_World->m_TileShapes.size(); i++)
	{
		CGraphObj obj;
		CDataFile f(m_World->m_TileShapes[i]->getFilename());
		CTexture **subset = getTextureSubset(m_World->m_TileShapes[i]->getSubset());
		printf("loading %s:\n", f.getName().c_str());
		obj.loadFromFile(&f, subset);
		m_Tiles.push_back(obj);
		delete [] subset;
	}

	CDataFile f(m_World->getBackgroundFilename());
	printf("Loading background %s:\n", f.getName().c_str());
	m_Background.setTextureSmooth(m_TexSmooth);
	m_Background.loadFromFile(f.useExtern(), 4*m_BackgroundSize, m_BackgroundSize);

	return true;
}

void CGraphicWorld::unloadWorld()
{
	; //TODO
}

bool CGraphicWorld::loadObjects()
{
	printf("Loading moving object graphics:\n");
	for(unsigned int i=0; i<m_World->m_MovObjBounds.size(); i++)
	{
		CGraphObj obj;
		CDataFile f(m_World->m_MovObjBounds[i]->getFilename());
		printf("Loading from %s\n", f.getName().c_str());
		obj.loadFromFile(&f, NULL);
		m_MovingObjects.push_back(obj);
	}

	return true;
}

void CGraphicWorld::unloadObjects()
{
}

CTexture **CGraphicWorld::getTextureSubset(CString indices)
{
	//printf("Indices: \"%s\"\n", indices.c_str());
	CTexture **ret = new (CTexture *)[1+indices.length()/2]; //We don't need more
	int i=0;
	while(true)
	{
		int sp = indices.inStr(' ');
		if(sp > 0)
		{
			int n = indices.mid(0,sp).toInt();
			indices= indices.mid(sp+1, indices.length()-sp-1);
			//printf("    Adding %d\n", n);
			CTexture *tex = &(m_TileTextures[n]);
			*(ret+i) = tex;
		}
		else
		{
			//printf("    Adding %d\n", indices.toInt());
			CTexture *tex = &(m_TileTextures[indices.toInt()]); //the last index
			*(ret+i) = tex;
			break;
		}

		i++;
	}

	return ret;
}
