/***************************************************************************
                          graphicworld.cpp  -  A graphical version of a world object
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

#include "graphicshape.h"
#include "graphicbound.h"
#include "graphicmaterial.h"

#include "graphicworld.h"

CGraphicWorld::CGraphicWorld(const CLConfig &conf) : CWorld(conf)
{
}

CGraphicWorld::~CGraphicWorld()
{
	//printf("Testing the textures: %d textures\n", m_TileMaterials.size());
	//for(int i=0; i<m_TileMaterials.size(); i++)
	//	( (CGraphicMaterial *)(m_TileMaterials[i]) )->getColor();
}

CShape *CGraphicWorld::createShape()
{return new CGraphicShape;}

CBound *CGraphicWorld::createBound()
{return new CGraphicBound;}

CMaterial *CGraphicWorld::createMaterial()
{return new CGraphicMaterial;}

bool CGraphicWorld::loadBackground(const CString &descr)
{
	printf("The background %s is being loaded\n", descr.c_str());
	m_Background.setTextureSmooth(m_TexSmooth);
	return m_Background.loadFromFile(descr, m_BackgroundSize, m_BackgroundSize / 4);
}
