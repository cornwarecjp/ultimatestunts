/***************************************************************************
                          reflection.cpp  -  A dynamic reflection map
                             -------------------
    begin                : ma sep 20 2004
    copyright            : (C) 2004 by CJP
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

#include "reflection.h"
#include "camera.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdlib>
#include <cstdio>

CReflection::CReflection(bool smooth)
{
	m_TexSmooth = smooth;
}

CReflection::~CReflection()
{
	//deleting reflections should be done by derived classes
}

void CReflection::enable(const SGraphicSettings *settings)
{
	m_CurrentSettings = settings;
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	GLfloat newambient[] = {1.0,1.0,1.0,1.0};
	glGetFloatv(GL_LIGHT_MODEL_AMBIENT, oldambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, newambient);

	glPolygonOffset(0.0, -1.0);


	glBindTexture(GL_TEXTURE_2D, m_Texture );
}

void CReflection::disable()
{
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, oldambient);
}

