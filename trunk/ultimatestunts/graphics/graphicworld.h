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

#include "texture.h"
#include "graphobj.h"
#include "background.h"
#include "world.h"
#include "lconfig.h"

/**
  *@author CJP
  */

class CGraphicWorld
{
public: 
	CGraphicWorld();
	virtual ~CGraphicWorld();

	bool loadWorld();
	void unloadWorld();
	bool loadObjects();
	void unloadObjects();

	vector<CGraphObj> m_Tiles;
	vector<CGraphObj> m_MovingObjects;

	CBackground m_Background;
	CTexture m_EnvMap;
protected:
	CTexture **getTextureSubset(CString indices);

	vector<CTexture> m_TileTextures;
	vector<CTexture> m_MovingObjectTextures;

	const CWorld *m_World;
	int m_TexMaxSize;
	int m_BackgroundSize;
	bool m_TexSmooth;
};

#endif
