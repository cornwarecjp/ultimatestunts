/***************************************************************************
                          background.cpp  -  description
                             -------------------
    begin                : di feb 4 2003
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

#include <math.h>

/* Some <math.h> files do not define M_PI... */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "background.h"

CBackground::CBackground(){
}
CBackground::~CBackground(){
}

bool CBackground::loadFromFile(CString filename, int xs, int ys)
{
	if(!(  CTexture::loadFromFile(filename, xs, ys)  )) return false;

	m_ObjList = glGenLists(1);
	glNewList(m_ObjList, GL_COMPILE);

	GLfloat white[] = {1.0, 1.0, 1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, white);

	CTexture::draw(1);

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);

	//sphere creation coming from a glutdemo (envmap.c)
	float r = 5.0, r1, r2, z1, z2;
	float theta1, theta2, phi;
	int nlon = 8, nlat = 4;
	int i=2, j=0;

#define TEXTUREMUL 4.0

	glBegin(GL_TRIANGLE_FAN);
	theta2 = 0.67*M_PI / nlat;
	r2 = r * sin(theta2);
	z2 = r * cos(theta2);
	glTexCoord2f(0.0, -TEXTUREMUL);
	glVertex3f(0.0, r,   0.0);
	for (j = 0, phi = 0.0; j <= nlon; j++, phi = 2 * M_PI * j / nlon) {
		glTexCoord2f((float)j/nlon, -TEXTUREMUL*(float)i/nlat);
		glVertex3f(r2 * cos(phi), z2, r2 * sin(phi));  // top
	}
	glEnd();
	for (i = 2; i < nlat; i++) {
		//Why 2/3 and not 1/2?? I don't understand.
		theta2 = 0.67*M_PI * i / nlat;
		theta1 = 0.67*M_PI * (i - 1) / nlat;
		r1 = r * sin(theta1);
		z1 = r * cos(theta1);
		r2 = r * sin(theta2);
		z2 = r * cos(theta2);
		glBegin(GL_QUAD_STRIP);
		for (j = 0, phi = 0; j <= nlon; j++, phi = 2 * M_PI * j / nlon) {
			glTexCoord2f((float)j/nlon, -TEXTUREMUL*(float)(i-1)/nlat);
			glVertex3f(r1 * cos(phi), z1, r1 * sin(phi));
			glTexCoord2f((float)j/nlon, -TEXTUREMUL*(float)i/nlat);
			glVertex3f(r2 * cos(phi), z2, r2 * sin(phi));
		}
		glEnd();
	}

	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);

	glEndList();

	return true;
}

void CBackground::draw() const
{
	glCallList(m_ObjList);
}
