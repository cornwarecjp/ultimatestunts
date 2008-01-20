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

#include "winsystem.h"
#include "usmacros.h"

#include "renderer.h"

CRenderer::CRenderer()
{
	m_FogColor = new float[4];

	reloadConfiguration();

	//And some other settings:
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 40);
}

bool CRenderer::reloadConfiguration()
{
	//Default values:
	m_Settings.m_UseBackground = true;
	m_Settings.m_ZBuffer = true;
	m_Settings.m_VisibleTiles = 10;
	m_Settings.m_FogMode = GL_EXP;
	m_Settings.m_Transparency = SGraphicSettings::blend;
	m_Settings.m_TexSmooth = true;
	m_Settings.m_ShadowSmooth = true;
	m_Settings.m_WaterTesselation = 10;
	m_Settings.m_EnableAnimation = true;
	m_Settings.m_ShadowSize = 512;
	m_Settings.m_CrashSmoke = true;

	//Load the setings
	CString cnf = theMainConfig->getValue("graphics", "background_size");
	//printf("Background size: %s\n", cnf.c_str());
	if(cnf != "" && cnf.toInt() <= 4)
		m_Settings.m_UseBackground = false;

	cnf = theMainConfig->getValue("graphics", "shadow_size");
	//printf("Shadow size: %s\n", cnf.c_str());
	if(cnf != "")
		m_Settings.m_ShadowSize = cnf.toInt();

	cnf = theMainConfig->getValue("graphics", "visible_tiles");
	//printf("Visible tiles: %s\n", cnf.c_str());
	if(cnf != "")
		m_Settings.m_VisibleTiles = cnf.toInt();

	cnf = theMainConfig->getValue("graphics", "zbuffer");
	//printf("Z buffer: %s\n", cnf.c_str());
	m_Settings.m_ZBuffer = (cnf != "false");

	cnf = theMainConfig->getValue("graphics", "texture_smooth");
	//printf("Smooth textures: %s\n", cnf.c_str());
	m_Settings.m_TexSmooth = (cnf != "false");

	cnf = theMainConfig->getValue("graphics", "shadows_smooth");
	//printf("Smooth shadows: %s\n", cnf.c_str());
	m_Settings.m_ShadowSmooth = (cnf != "false");

	cnf = theMainConfig->getValue("graphics", "fogmode");
	//printf("Fog mode: %s\n", cnf.c_str());
	if(cnf != "")
	{
		if(cnf == "off")	m_Settings.m_FogMode = -1;
		if(cnf == "linear")	m_Settings.m_FogMode = GL_LINEAR;
		if(cnf == "exp")	m_Settings.m_FogMode = GL_EXP;
		if(cnf == "exp2")	m_Settings.m_FogMode = GL_EXP2;
	}

	cnf = theMainConfig->getValue("graphics", "transparency");
	//printf("Transparency: %s\n", cnf.c_str());
	if(cnf != "")
	{
		if(cnf == "off")	m_Settings.m_Transparency = SGraphicSettings::off;
		if(cnf == "blend")	m_Settings.m_Transparency = SGraphicSettings::blend;
	}

	cnf = theMainConfig->getValue("animation", "watertesselation");
	if(cnf != "")
	{
		m_Settings.m_WaterTesselation = cnf.toInt();
		if(cnf.toInt() < 0) m_Settings.m_WaterTesselation = 0;
	}

	m_Settings.m_ReflectionSize = theMainConfig->getValue("graphics", "reflection_size").toInt();
	m_Settings.m_ReflectionDist = theMainConfig->getValue("graphics", "reflectiondist").toFloat();
	m_Settings.m_UpdRef = theMainConfig->getValue("graphics", "updatereflection") == "true";
	m_Settings.m_UpdRefAllSides = theMainConfig->getValue("graphics", "updatereflectionallsides") == "true";
	m_Settings.m_UpdRefAllObjs = theMainConfig->getValue("graphics", "updatereflectionallobjects") == "true";
	m_Settings.m_ReflectionDrawMovingObjects = theMainConfig->getValue("graphics", "reflectiondrawmovingobjects") == "true";

	m_Settings.m_MovingObjectLOD = theMainConfig->getValue("graphics", "movingobjectlod").toInt();

	m_Settings.m_TrackDisplayList = theMainConfig->getValue("graphics", "trackdisplaylist") == "true";

	m_Settings.m_EnableAnimation = theMainConfig->getValue("animation", "enable") == "true";

	m_Settings.m_CrashSmoke = theMainConfig->getValue("animation", "crashsmoke") == "true";

	//Next: use these settings
	if(m_Settings.m_ZBuffer)
		{glEnable(GL_DEPTH_TEST);}
	else
		{glDisable(GL_DEPTH_TEST);}

	if(m_Settings.m_FogMode  < 0)
		{glDisable(GL_FOG);}
	else
	{
		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, m_Settings.m_FogMode);
		glFogf(GL_FOG_DENSITY, 2.0/(float)(TILESIZE*m_Settings.m_VisibleTiles));
		glFogi(GL_FOG_START, 0);
		glFogi(GL_FOG_END, TILESIZE*m_Settings.m_VisibleTiles);
	}

	if(m_Settings.m_Transparency == SGraphicSettings::blend)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}

	/*
	if(m_Settings.m_TexSmooth)
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

	if(m_Settings.m_ShadowSmooth)
		glShadeModel( GL_SMOOTH );
	else
		glShadeModel( GL_FLAT );

	return true;
}

CRenderer::~CRenderer()
{
	delete [] m_FogColor;
}

void CRenderer::update()
{
	updateScreenSize();

	//Set up openGL viewport + frustum stuff to window size

	float ratio = (float) m_W / (float) m_H;
	GLfloat near = 1.0;
	GLfloat far = TILESIZE * m_Settings.m_VisibleTiles;
	float hor_mul = near / 5.0;
	GLfloat xs = ratio*hor_mul;
	GLfloat ys = 1.0*hor_mul;

	//printf("glViewport(%d, %d, %d, %d);\n", m_X, m_Y, m_W, m_H);
	glViewport( m_X, m_Y, m_W, m_H );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glFrustum( -xs, xs, -ys, ys, near, far );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

void CRenderer::updateScreenSize()
{
	m_X = m_Y = 0;
	m_W = theWinSystem->getWidth();
	m_H = theWinSystem->getHeight();
}
