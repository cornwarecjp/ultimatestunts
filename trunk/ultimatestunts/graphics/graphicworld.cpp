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

#include "graphicworld.h"
#include "datafile.h"

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
}

CGraphicWorld::~CGraphicWorld()
{
	unloadWorld();
	unloadObjects();
}

bool CGraphicWorld::loadWorld()
{
	printf("Loading the graphic world\n");

	printf("  Loading tile textures:\n");
	for(unsigned int i=0; i<m_World->m_TileMaterials.size(); i++)
	{
		CLODTexture t;
		CDataFile f(m_World->m_TileMaterials[i]->m_Filename);
		int mul = m_World->m_TileMaterials[i]->m_Mul;
		printf("   Loading %s with mul=%d:\n", f.getName().c_str(), mul);
		t.setTextureSmooth(m_TexSmooth);
		int xs = m_TexMaxSize / mul;
		int ys = m_TexMaxSize / mul;
		t.loadFromFile(f.useExtern(), xs, ys);
		m_TileTextures.push_back(t);
	}

	printf("  Loading tiles:\n");
	for(unsigned int i=0; i<m_World->m_TileModels.size(); i++)
	{
		CGraphObj obj;
		CFile f(m_World->m_TileModels[i]->m_Filename);
		CLODTexture **subset = getTextureSubset(m_World->m_TileModels[i]->m_Subset);
		printf("   Loading %s:\n", f.getName().c_str());
		obj.loadFromFile(&f, subset);
		m_Tiles.push_back(obj);
		delete [] subset;
	}

	CDataFile fb(m_World->getBackgroundFilename());
	printf("  Loading background %s:\n", fb.getName().c_str());
	m_Background.setTextureSmooth(m_TexSmooth);
	m_Background.loadFromFile(fb.useExtern(), 4*m_BackgroundSize, m_BackgroundSize);

	CDataFile fe(m_World->getEnvMapFilename());
	printf("  Loading environment map %s:\n", fe.getName().c_str());
	m_EnvMap.setTextureSmooth(m_TexSmooth);
	m_EnvMap.loadFromFile(fe.useExtern(), m_BackgroundSize, m_BackgroundSize);

	return true;
}

void CGraphicWorld::unloadWorld()
{
	printf("Unloading the graphic world\n");

	printf("  Unloading background\n");
	m_Background.unload();

	printf("  Unloading environment map\n");
	m_EnvMap.unload();

	printf("  Unloading tiles:\n");
	for(unsigned int i=0; i<m_Tiles.size(); i++)
		m_Tiles[i].unload();
	m_Tiles.clear();

	printf("  Unloading tile textures:\n");
	for(unsigned int i=0; i<m_TileTextures.size(); i++)
		m_TileTextures[i].unload();
	m_TileTextures.clear();
}

bool CGraphicWorld::loadObjects()
{
	int molod = theMainConfig->getValue("graphics", "movingobjectlod").toInt();

	//Body graphics
	printf("Loading moving object graphics:\n");
	for(unsigned int i=0; i<m_World->m_MovObjBounds.size(); i++)
	{
		CGraphObj obj;
		CFile f(m_World->m_MovObjBounds[i]->m_Filename);
		printf("   Loading from %s LOD %d\n", f.getName().c_str(), molod);
		obj.loadFromFile(&f, NULL, molod);
		m_MovingObjects.push_back(obj);

	}

	return true;
}

void CGraphicWorld::unloadObjects()
{
	printf("Unloading moving object graphics:\n");
	for(unsigned int i=0; i<m_MovingObjects.size(); i++)
		m_MovingObjects[i].unload();
	m_MovingObjects.clear();
}

CLODTexture **CGraphicWorld::getTextureSubset(CString indices)
{
	//printf("Indices: \"%s\"\n", indices.c_str());
	CLODTexture **ret = new (CLODTexture *)[1+indices.length()/2]; //We don't need more
	int i=0;
	while(true)
	{
		int sp = indices.inStr(' ');
		if(sp > 0)
		{
			int n = indices.mid(0,sp).toInt();
			indices= indices.mid(sp+1, indices.length()-sp-1);
			//printf("    Adding %d\n", n);
			CLODTexture *tex = &(m_TileTextures[n]);
			*(ret+i) = tex;
		}
		else
		{
			//printf("    Adding %d\n", indices.toInt());
			CLODTexture *tex = &(m_TileTextures[indices.toInt()]); //the last index
			*(ret+i) = tex;
			break;
		}

		i++;
	}

	return ret;
}
