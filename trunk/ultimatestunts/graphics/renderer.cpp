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
#include <cstdio>
#include <cmath>
#include <GL/gl.h>

#include "renderer.h"
#include "usmacros.h"

CRenderer::CRenderer(const CWinSystem *winsys)
{
	m_WinSys = winsys;
	
	m_FogColor = new float[4];

	//Default values:
	m_UseBackground = true;
	m_ZBuffer = true;
	m_VisibleTiles = 10;
	m_FogMode = GL_EXP;
	m_Transparency = blend;
	m_TexSmooth = true;
	m_ShadowSmooth = true;

	//Load the setings
	CString cnf = theMainConfig->getValue("graphics", "background_size");
	//printf("Background size: %s\n", cnf.c_str());
	if(cnf != "" && cnf.toInt() <= 4)
		m_UseBackground = false;

	cnf = theMainConfig->getValue("graphics", "visible_tiles");
	//printf("Visible tiles: %s\n", cnf.c_str());
	if(cnf != "")
		m_VisibleTiles = cnf.toInt();

	cnf = theMainConfig->getValue("graphics", "zbuffer");
	//printf("Z buffer: %s\n", cnf.c_str());
	m_ZBuffer = (cnf != "false");

	cnf = theMainConfig->getValue("graphics", "texture_smooth");
	//printf("Smooth textures: %s\n", cnf.c_str());
	m_TexSmooth = (cnf != "false");

	cnf = theMainConfig->getValue("graphics", "shadows_smooth");
	//printf("Smooth shadows: %s\n", cnf.c_str());
	m_ShadowSmooth = (cnf != "false");

	cnf = theMainConfig->getValue("graphics", "fogmode");
	//printf("Fog mode: %s\n", cnf.c_str());
	if(cnf != "")
	{
		if(cnf == "off")	m_FogMode = -1;
		if(cnf == "linear")	m_FogMode = GL_LINEAR;
		if(cnf == "exp")	m_FogMode = GL_EXP;
		if(cnf == "exp2")	m_FogMode = GL_EXP2;
	}

	cnf = theMainConfig->getValue("graphics", "transparency");
	//printf("Transparency: %s\n", cnf.c_str());
	if(cnf != "")
	{
		if(cnf == "off")	m_Transparency = off;
		if(cnf == "blend")	m_Transparency = blend;
	}

	m_ReflectionSize = theMainConfig->getValue("graphics", "reflection_size").toInt();
	m_ReflectionDist = theMainConfig->getValue("graphics", "reflectiondist").toFloat();
	m_ReflectionUpdateFrames = theMainConfig->getValue("graphics", "reflectionupdateframes").toInt();
	m_ReflectionSkipMovingObjects = theMainConfig->getValue("graphics", "reflectionskipmovingobjects") == "true";

	m_MovingObjectLOD = theMainConfig->getValue("graphics", "movingobjectlod").toInt();

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

	if(m_Transparency == blend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}

	/*
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
	*/

	if(m_ShadowSmooth)
		glShadeModel( GL_SMOOTH );
	else
		glShadeModel( GL_FLAT );

	//And some other settings:
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

CRenderer::~CRenderer()
{
	delete [] m_FogColor;
}

void CRenderer::update()
{
	unsigned int w = m_WinSys->getWidth();
	unsigned int h = m_WinSys->getHeight();

	//Set up openGL viewport + frustum stuff to window size

	float ratio = (float) w / (float) h;
	GLfloat near = 1.0;
	GLfloat far = TILESIZE * m_VisibleTiles;
	float hor_mul = near / 5.0;
	GLfloat xs = ratio*hor_mul;
	GLfloat ys = 1.0*hor_mul;

	glViewport( 0, 0, w, h );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glFrustum( -xs, xs, -ys, ys, near, far );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

