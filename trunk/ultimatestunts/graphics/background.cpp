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

#include <cmath>

/* Some <cmath> files do not define M_PI... */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "background.h"

CBackground::CBackground(){
}
CBackground::~CBackground(){
}

void CBackground::draw() const
{
	float hmax = 1000.0;
	float vmax = 10.0;
	float tiling = 10.0;

	float t = 0.01 * m_Timer.getTime();
	float drift = t - (float)((int)t);

	GLfloat color[] = {1.0, 1.0, 1.0};
	if(getSizeX(1) <=4 || getSizeY(1) <= 4)
	{
		color[0] = m_Color.x;
		color[1] = m_Color.y;
		color[2] = m_Color.z;
	}
	else
		{CTexture::draw(1);}

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);

	glNormal3f(0.0, -1.0, 0.0);
	glBegin(GL_TRIANGLE_FAN);

	glTexCoord2f(drift, drift);
	glVertex3f(0.0, vmax, 0.0);

	glTexCoord2f(drift, -tiling + drift);
	glVertex3f(0.0, vmax, -hmax);

	glTexCoord2f(-tiling + drift, -tiling + drift);
	glVertex3f(-hmax, vmax, -hmax);

	glTexCoord2f(-tiling + drift, drift);
	glVertex3f(-hmax, vmax, 0.0);

	glTexCoord2f(-tiling + drift, tiling + drift);
	glVertex3f(-hmax, vmax, hmax);

	glTexCoord2f(drift, tiling + drift);
	glVertex3f(0.0, vmax, hmax);

	glTexCoord2f(tiling + drift, tiling + drift);
	glVertex3f(hmax, vmax, hmax);

	glTexCoord2f(tiling + drift, drift);
	glVertex3f(hmax, vmax, 0.0);

	glTexCoord2f(tiling + drift, -tiling + drift);
	glVertex3f(hmax, vmax, -hmax);

	glTexCoord2f(drift, -tiling + drift);
	glVertex3f(0.0, vmax, -hmax);

	glEnd();
	
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
}
