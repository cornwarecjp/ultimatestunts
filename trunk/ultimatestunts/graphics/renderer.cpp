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
#include <math.h>
#include <GL/gl.h>

#include "renderer.h"
#include "usmacros.h"

CRenderer::CRenderer(const CLConfig &conf)
{
	m_FogColor = new float[4];

	//Default values:
	m_UseBackground = true;
	m_ZBuffer = true;
	m_VisibleTiles = 10;
	m_FogMode = GL_EXP;
	m_TexPerspective = true;
	m_TexSmooth = true;
	m_ShadowSmooth = true;

	//Load the setings
	CString cnf = conf.getValue("graphics", "background_size");
	//printf("Background size: %s\n", cnf.c_str());
	if(cnf != "" && cnf.toInt() <= 4)
		m_UseBackground = false;

	cnf = conf.getValue("graphics", "visible_tiles");
	//printf("Visible tiles: %s\n", cnf.c_str());
	if(cnf != "")
		m_VisibleTiles = cnf.toInt();

	cnf = conf.getValue("graphics", "texture_perspective");
	//printf("Texture perspective: %s\n", cnf.c_str());
	m_TexPerspective = (cnf != "false");

	cnf = conf.getValue("graphics", "zbuffer");
	//printf("Z buffer: %s\n", cnf.c_str());
	m_ZBuffer = (cnf != "false");

	cnf = conf.getValue("graphics", "texture_smooth");
	//printf("Smooth textures: %s\n", cnf.c_str());
	m_TexSmooth = (cnf != "false");

	cnf = conf.getValue("graphics", "shadows_smooth");
	//printf("Smooth shadows: %s\n", cnf.c_str());
	m_ShadowSmooth = (cnf != "false");

	cnf = conf.getValue("graphics", "fogmode");
	//printf("Fog mode: %s\n", cnf.c_str());
	if(cnf != "")
	{
		if(cnf == "off")		m_FogMode = -1;
		if(cnf == "linear")	m_FogMode = GL_LINEAR;
		if(cnf == "exp")		m_FogMode = GL_EXP;
		if(cnf == "exp2")	m_FogMode = GL_EXP2;
	}

	//Next: use these settings
	if(m_ZBuffer)
		{glEnable(GL_DEPTH_TEST);}
	else
		{glDisable(GL_DEPTH_TEST);}

	if(m_FogMode  < 0)
		{glDisable(GL_FOG);}
	else
	{
		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, m_FogMode);
		glFogf(GL_FOG_DENSITY, 2.0/(float)(TILESIZE*m_VisibleTiles));
		glFogi(GL_FOG_START, 0);
		glFogi(GL_FOG_END, TILESIZE*m_VisibleTiles);
	}

	if(m_TexSmooth)
	{
		glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );
		glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	}
	else
	{
		glHint( GL_POINT_SMOOTH_HINT, GL_FASTEST );
		glHint( GL_POLYGON_SMOOTH_HINT, GL_FASTEST );
	}

	if(m_TexPerspective)
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	else
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	if(m_ShadowSmooth)
		glShadeModel( GL_SMOOTH );
	else
		glShadeModel( GL_FLAT );

	//And some other settings:
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
}

CRenderer::~CRenderer()
{
	delete [] m_FogColor;
}

void CRenderer::update()
{
}

