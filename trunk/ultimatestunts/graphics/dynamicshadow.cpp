/***************************************************************************
                          dynamicshadow.cpp  -  An updatable shadow projection texture
                             -------------------
    begin                : do jan 26 2006
    copyright            : (C) 2006 by CJP
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
#include <GL/glu.h>
#include <cstdlib>
#include <cstdio>

#include "usmacros.h"

#include "world.h"
#include "graphicworld.h"
#include "collisionmodel.h"

#include "dynamicshadow.h"

CDynamicShadow::CDynamicShadow(unsigned int size, CGraphicWorld *graphworld, unsigned int objID)
{
	m_Size = size;
	m_PhysicalSize = 2.0;
	initialiseShadow();

	m_GraphicWorld = graphworld;
	m_MovObjID = objID;
}

CDynamicShadow::~CDynamicShadow()
{
	glDeleteTextures(1, &m_Texture);
}

void CDynamicShadow::initialiseShadow()
{
	char *data = new char[3*m_Size*m_Size];

	for(unsigned int x=0; x < m_Size; x++)
		for(unsigned int y=0; y < m_Size; y++)
		{
			unsigned int i = m_Size*x + y;
			int dx = int(x) - m_Size/2;
			int dy = int(y) - m_Size/2;
			unsigned int r2 = dx*dx + dy*dy;

			char val = 0;
			if(r2 < m_Size*m_Size/5)
				val = 128;

			data[3*i+0] = val;
			data[3*i+1] = val;
			data[3*i+2] = val;
		}

	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexImage2D( GL_TEXTURE_2D, 0, 3, m_Size, m_Size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	delete [] data;
}

bool CDynamicShadow_fEnabled = true;

void CDynamicShadow::enable()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_Texture);

	/*
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	*/

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(0.0, -2.0);

	glDisable(GL_LIGHTING);

	bool CDynamicShadow_fEnabled = glIsEnabled(GL_FOG);
	if(CDynamicShadow_fEnabled) glDisable(GL_FOG);

	//Special blending function (think about this one):
	//white texels -> opaque
	//black texels -> transparent
	glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
}

void CDynamicShadow::disable()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if(CDynamicShadow_fEnabled) glEnable(GL_FOG);

	glEnable(GL_LIGHTING);

	glDisable(GL_POLYGON_OFFSET_FILL);

	/*
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	*/
}

void CDynamicShadow::update(const SGraphicSettings *settings, float t)
{
	CMovingObject *mo = theWorld->getMovingObject(m_MovObjID);
	CCollisionModel *collmod = (CCollisionModel *)theWorld->getObject(CDataObject::eBound, mo->m_Bodies[0].m_Body);
	float Rsphere = collmod->m_BSphere_r;

	m_PhysicalSize = Rsphere;

	//no perspective
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(-Rsphere, Rsphere,-Rsphere, Rsphere,-Rsphere, Rsphere);
	glMatrixMode( GL_MODELVIEW );


	//set shadow size viewport
	glEnable(GL_SCISSOR_TEST);
	glViewport(0, 0, m_Size, m_Size);
	glScissor(0, 0, m_Size, m_Size);

	glLoadMatrixf(m_LightOrientation.transpose().gl_mtr());

	bool zEnabled = glIsEnabled(GL_DEPTH_TEST);
	if(zEnabled) glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor3f(m_ShadowColor.x, m_ShadowColor.y, m_ShadowColor.z);

	//Clear:
	float oldClear[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, oldClear);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);

	//Draw the model:
	CVector pos0 = mo->m_Bodies[0].m_Position;
	for(unsigned int i=0; i < mo->m_Bodies.size(); i++)
	{
		CBody &b = mo->m_Bodies[i];
		CVector r = b.m_Position - pos0;

		int lod = 1;
		lod += settings->m_MovingObjectLOD;
		if(lod > 4) lod = 4;
		if(lod < 1) lod = 1;

		glPushMatrix();
		glTranslatef (r.x, r.y, r.z);
		glMultMatrixf(b.m_OrientationMatrix.gl_mtr());

		//The model
		m_GraphicWorld->getMovObjBound(b.m_Body)->draw(settings, NULL, lod, t, false);

		glPopMatrix();
	}

	glClearColor(oldClear[0],oldClear[1],oldClear[2],oldClear[3]);

	glColor3f(1,1,1);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	if(zEnabled) glEnable(GL_DEPTH_TEST);

	glDisable(GL_SCISSOR_TEST);

	//Now copy the result to the shadow texture.
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	CopyToTexture(m_Size);
}
