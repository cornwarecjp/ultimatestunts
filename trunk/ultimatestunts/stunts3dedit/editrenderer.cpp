/***************************************************************************
                          editrenderer.cpp  -  The renderer of stunts3dedit
                             -------------------
    begin                : wo apr 9 2003
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
#include <GL/gl.h>

#include "editrenderer.h"
#include "usmacros.h"
#include "datafile.h"

CEditRenderer::CEditRenderer(const CWinSystem *winsys) : CRenderer(winsys)
{
	m_EnvMap = new CTexture;
	CDataFile df("environment/spheremap.rgb");
	m_EnvMap->loadFromFile(df.useExtern(), m_ReflectionSize, m_ReflectionSize);
}

CEditRenderer::~CEditRenderer()
{
	delete m_EnvMap;
}

void CEditRenderer::update()
{
	CRenderer::update(); //set up viewport
	//printf("Updating graphics\n");

	//Clear the screen
	if(m_ZBuffer)
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	else
		glClear( GL_COLOR_BUFFER_BIT );

	//Set the camera
	const CMatrix &cammat = m_Camera->getOrientation();
	glLoadMatrixf(cammat.transpose().gl_mtr());
	const CVector &camera = m_Camera->getPosition();
	glTranslatef (-camera.x, -camera.y, -camera.z);

	//The model
	m_GraphObj->draw(1);

	//The reflection
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	GLfloat oldambient[4];
	GLfloat newambient[] = {1.0,1.0,1.0,1.0};
	glGetFloatv(GL_LIGHT_MODEL_AMBIENT, oldambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, newambient);
	glPolygonOffset(0.0, -1.0);

	m_EnvMap->draw();
	m_GraphObj->draw(0);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, oldambient);

	//The coordinate axes
	float kleur[] = {1.0, 1.0, 1.0, 1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, kleur);

	glBegin(GL_LINES);
	glVertex3f(-TILESIZE/2,0.0,0.0);
	glVertex3f(TILESIZE/2,0.0,0.0);

	glVertex3f(0.0,0.0,-TILESIZE/2);
	glVertex3f(0.0,0.0,TILESIZE/2);

	glVertex3f(0.0,0.0,0.0);
	glVertex3f(0.0,VERTSIZE,0.0);

	glEnd();
}
