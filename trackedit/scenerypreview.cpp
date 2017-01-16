/***************************************************************************
                          scenerypreview.cpp  -  A widget for previewing track scenery
                             -------------------
    begin                : wo dec 22 2010
    copyright            : (C) 2010 by CJP
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

#include <cmath>
#include "pi.h"

#include <GL/gl.h>

#include "scenerypreview.h"

CSceneryPreview::CSceneryPreview() :
	m_SkyTexture(NULL),
	m_BackgroundTexture(NULL),
	m_SunTexture(NULL)
{
	m_SunTexture.load("misc/sun.rgba", CParamList());
}


CSceneryPreview::~CSceneryPreview()
{
}


int CSceneryPreview::onRedraw()
{
	CWidget::onRedraw();

	//The air color
	glBindTexture(GL_TEXTURE_2D, 0); //no texture
	glBegin(GL_QUADS);

	setColor(m_Environment.m_SkyColor);
	glVertex2f(m_W-1,m_H-1);
	glVertex2f(1,m_H-1);
	setColor(m_Environment.m_HorizonSkyColor);
	glVertex2f(1,m_H/4);
	glVertex2f(m_W-1,m_H/4);

	glEnd();

	//The light
	m_SunTexture.draw();
	setColor(m_Environment.m_LightColor);
	float lightY = -m_Environment.m_LightDirection.normal().y;
	lightY = (0.25 + 0.75*lightY)*m_H;
	float lightX = atan2f(
		-m_Environment.m_LightDirection.x,
		 m_Environment.m_LightDirection.z);
	lightX = 0.5*(1.0 + lightX/M_PI)*m_W;
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2f(lightX-31,lightY-31);
	glTexCoord2f(1,0);
	glVertex2f(lightX+32,lightY-31);
	glTexCoord2f(1,1);
	glVertex2f(lightX+32,lightY+32);
	glTexCoord2f(0,1);
	glVertex2f(lightX-31,lightY+32);
	glEnd();

	setColor(m_Environment.m_EnvironmentColor);

	//The sky (clouds):
	const float xrepeat = 2.0, yrepeat = 4.0;
	if(m_SkyTexture.getFilename() != m_Environment.m_SkyFilename)
	{
		//TODO: set resolution properly
		m_SkyTexture.load(m_Environment.m_SkyFilename, CParamList());
	}
	m_SkyTexture.draw();
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2f(1,m_H/4);
	glTexCoord2f(xrepeat,0);
	glVertex2f(m_W-1,m_H/4);
	glTexCoord2f(xrepeat,yrepeat);
	glVertex2f(m_W-1,m_H-1);
	glTexCoord2f(0,yrepeat);
	glVertex2f(1,m_H-1);
	glEnd();

	//The background image:
	if(m_BackgroundTexture.getFilename() != m_Environment.m_HorizonFilename)
	{
		//TODO: set resolution properly
		m_BackgroundTexture.load(m_Environment.m_HorizonFilename, CParamList());
	}
	m_BackgroundTexture.draw();
	glBegin(GL_QUADS);
	glTexCoord2f(0,0.5);
	glVertex2f(1,m_H/4);
	glTexCoord2f(1,0.5);
	glVertex2f(m_W-1,m_H/4);
	glTexCoord2f(1,1);
	glVertex2f(m_W-1,m_H/2);
	glTexCoord2f(0,1);
	glVertex2f(1,m_H/2);
	glEnd();

	//The ground
	glBindTexture(GL_TEXTURE_2D, 0); //no texture
	glBegin(GL_QUADS);
	CVector groundColor =
		m_Environment.m_LightColor *
		-m_Environment.m_LightDirection.normal().y
		+ m_Environment.m_AmbientColor;
	groundColor.x *= 0.4;
	groundColor.y *= 0.5;
	groundColor.z *= 0.3;
	setColor(groundColor);
	glVertex2f(1,1);
	glVertex2f(m_W-1,1);
	setColor(m_Environment.m_FogColor);
	glVertex2f(m_W-1,m_H/4);
	glVertex2f(1,m_H/4);
	glEnd();

	return 0;
}

void CSceneryPreview::setColor(const CVector &color) const
{
	glColor3f(color.x, color.y, color.z);
}

