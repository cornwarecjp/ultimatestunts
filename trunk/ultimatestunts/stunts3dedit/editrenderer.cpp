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

CEditRenderer::CEditRenderer(const CLConfig &conf) : CRenderer(conf)
{
}

CEditRenderer::~CEditRenderer(){
}

void CEditRenderer::update()
{
	//printf("Updating graphics\n");

	if(m_ZBuffer)
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	else
		glClear( GL_COLOR_BUFFER_BIT );

	const CMatrix &cammat = m_Camera->getOrientation();
	//glLoadIdentity();
	glLoadMatrixf(cammat.inverse().gl_mtr());

	const CVector &camera = m_Camera->getPosition();

	glTranslatef (-camera.x, -camera.y, -camera.z);

	int lod = 1;
	m_GraphObj->draw(lod);

	glBegin(GL_LINES);
	glVertex3f(-TILESIZE/2,0.0,0.0);
	glVertex3f(TILESIZE/2,0.0,0.0);

	glVertex3f(0.0,0.0,-TILESIZE/2);
	glVertex3f(0.0,0.0,TILESIZE/2);

	glVertex3f(0.0,0.0,0.0);
	glVertex3f(0.0,VERTSIZE,0.0);

	glEnd();
}
