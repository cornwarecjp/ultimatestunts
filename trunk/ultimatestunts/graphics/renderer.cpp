/***************************************************************************
                          renderer.cpp  -  The graphics rendering routines
                             -------------------
    begin                : di jan 28 2003
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
#include <GL/gl.h>

#include "renderer.h"

CRenderer::CRenderer(const CLConfig &conf, CGraphicWorld *world)
{
	printf("Renderer created\n");
	m_World = world;
}

CRenderer::~CRenderer(){
}

void CRenderer::Update()
{
	printf("Yeah! I'm updating the graphics!\n");
}
