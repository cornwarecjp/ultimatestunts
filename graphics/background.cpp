/***************************************************************************
                          background.cpp  -  A sky box
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
#include "pi.h"

#include "lconfig.h"

#include "datafile.h"

#include "background.h"

CBackground::CBackground(CDataManager *manager) : CTexture(manager)
{}

CBackground::~CBackground(){
}

bool CBackground::load(const CString &filename, const CParamList &list)
{
	if(!CTexture::load(filename, list)) return false;

	m_ShowGradient = theMainConfig->getValue("graphics", "shadows_smooth") == "true";

	m_SkyColor = m_ParamList.getValue("skycol", "0,0,0").toVector();
	m_HorizonSkyColor = m_ParamList.getValue("horizonskycol", "0,0,0").toVector();
	m_FogColor = m_ParamList.getValue("fogcol", "0,0,0").toVector();
	m_EnvironmentColor = m_ParamList.getValue("envcol", "0,0,0").toVector();

	RGBImageRec *image = NULL;
	{
		CDataFile f(m_ParamList.getValue("horizon", "[NO HORIZON FILENAME PROVIDED]"));
		RGBImageRec *in_image = loadImage(f.useExtern());
	
		int sx = list.getValue("sizex", "256").toInt();
		int sy = list.getValue("sizey", "256").toInt();

		m_ShowTextures = sx > 4 && sy > 4;

		image = scaleImage(in_image, sx, sy);
		if(image==NULL)
		{
			image = in_image; //in_image is not freed because we use it
		}
		else
		{
			freeImage(in_image); //no longer used because we have image
		}
	}

	glGenTextures(1, &m_HorizonTex);
	glBindTexture(GL_TEXTURE_2D, m_HorizonTex);

	if(image->format == 1) //RGBA
		{glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, image->sizeX, image->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);}
	else //Assume RGB
		{glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8,  image->sizeX, image->sizeY, 0, GL_RGB,  GL_UNSIGNED_BYTE, image->data);}

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	if(m_TextureSmooth)
	{
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else
	{
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	}

	freeImage(image);

	return true;
}

void CBackground::unload()
{
	if(!isLoaded()) return;

	glDeleteTextures(1, &m_HorizonTex);
	m_HorizonTex = 0;

	CTexture::unload();
}

CVector CBackground::getClearColor() const
{
	return 0.5*(m_HorizonSkyColor + m_SkyColor);
}


void CBackground::draw() const
{
	if(!m_ShowTextures && !m_ShowGradient) return;

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);

	bool fogEnabled = glIsEnabled(GL_FOG);
	glDisable(GL_FOG);

	if(m_ShowGradient)
		drawSkybox();

	if(m_ShowTextures)
	{
		glColor3f(m_EnvironmentColor.x, m_EnvironmentColor.y, m_EnvironmentColor.z);

		drawClouds();
		drawHorizon();

		glColor3f(1,1,1);
	}

	if(fogEnabled) glEnable(GL_FOG);

	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
}

void CBackground::drawSkybox() const
{
	float vmax = 10.0;
	float hmax = 10.0*vmax;

	glBindTexture(GL_TEXTURE_2D, 0);

	glBegin(GL_TRIANGLE_FAN);

	glColor3f(m_SkyColor.x, m_SkyColor.y, m_SkyColor.z);
	glVertex3f(0.0, vmax, 0.0);

	glColor3f(m_HorizonSkyColor.x, m_HorizonSkyColor.y, m_HorizonSkyColor.z);

	glVertex3f(0.0, 0.0, -hmax);
	glVertex3f(-hmax, 0.0, -hmax);
	glVertex3f(-hmax, 0.0, 0.0);
	glVertex3f(-hmax, 0.0, hmax);
	glVertex3f(0.0, 0.0, hmax);
	glVertex3f(hmax, 0.0, hmax);
	glVertex3f(hmax, 0.0, 0.0);
	glVertex3f(hmax, 0.0, -hmax);
	glVertex3f(0.0, 0.0, -hmax);

	glEnd();
}

void CBackground::drawClouds() const
{
	float vmax = 10.0;
	float hmax = 100.0*vmax;
	float tiling = 10.0;

	float t = 0.01 * m_Timer.getTime();
	float drift = t - (float)((int)t);

	CTexture::draw();

	glNormal3f(0.0, -1.0, 0.0); //not really necessary, because there's no lighting

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
}

void CBackground::drawHorizon() const
{
	float vmax = 10.0;
	//float hmax = 100.0*vmax;

	glBindTexture(GL_TEXTURE_2D, m_HorizonTex);

	glBegin(GL_QUADS);

	//North
	glTexCoord2f(0,0);
	glVertex3f(-vmax,-0.05*vmax,-vmax);
	glTexCoord2f(2,0);
	glVertex3f( vmax,-0.05*vmax,-vmax);
	glTexCoord2f(2,1);
	glVertex3f( vmax, 0.05*vmax,-vmax);
	glTexCoord2f(0,1);
	glVertex3f(-vmax, 0.05*vmax,-vmax);

	//South
	glTexCoord2f(0,0);
	glVertex3f( vmax,-0.05*vmax,vmax);
	glTexCoord2f(2,0);
	glVertex3f(-vmax,-0.05*vmax,vmax);
	glTexCoord2f(2,1);
	glVertex3f(-vmax, 0.05*vmax,vmax);
	glTexCoord2f(0,1);
	glVertex3f( vmax, 0.05*vmax,vmax);

	//West
	glTexCoord2f(0,0);
	glVertex3f(-vmax,-0.05*vmax,vmax);
	glTexCoord2f(2,0);
	glVertex3f(-vmax,-0.05*vmax,-vmax);
	glTexCoord2f(2,1);
	glVertex3f(-vmax, 0.05*vmax,-vmax);
	glTexCoord2f(0,1);
	glVertex3f(-vmax, 0.05*vmax,vmax);

	//East
	glTexCoord2f(0,0);
	glVertex3f(vmax,-0.05*vmax,-vmax);
	glTexCoord2f(2,0);
	glVertex3f(vmax,-0.05*vmax,vmax);
	glTexCoord2f(2,1);
	glVertex3f(vmax, 0.05*vmax,vmax);
	glTexCoord2f(0,1);
	glVertex3f(vmax, 0.05*vmax,-vmax);

	glEnd();
}

