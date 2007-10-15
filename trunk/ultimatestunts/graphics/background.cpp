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

#include "datafile.h"

#include "background.h"

CBackground::CBackground(CDataManager *manager) : CTexture(manager)
{}

CBackground::~CBackground(){
}

bool CBackground::load(const CString &filename, const CParamList &list)
{
	if(!CTexture::load(filename, list)) return false;

	m_SkyColor = m_ParamList.getValue("skycolor", "0,0,0").toVector();

	RGBImageRec *image = NULL;
	{
		CDataFile f(m_ParamList.getValue("horizon", "[NO HORIZON FILENAME PROVIDED]"));
		RGBImageRec *in_image = loadImage(f.useExtern());
	
		int sx = list.getValue("sizex", "256").toInt();
		int sy = list.getValue("sizey", "256").toInt();

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

	//determine color
	RGBImageRec *image2 = scaleImage(image, 1,1);
	if(image2==NULL)
	{
		m_HorizonColor.x = (float)*(image->data  ) / 255;
		m_HorizonColor.y = (float)*(image->data+1) / 255;
		m_HorizonColor.z = (float)*(image->data+2) / 255;
	}
	else
	{
		m_HorizonColor.x = (float)*(image2->data  ) / 255;
		m_HorizonColor.y = (float)*(image2->data+1) / 255;
		m_HorizonColor.z = (float)*(image2->data+2) / 255;

		freeImage(image2);
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

void CBackground::draw() const
{
	float vmax = 10.0;
	float hmax = 100.0*vmax;
	float tiling = 10.0;

	float t = 0.01 * m_Timer.getTime();
	float drift = t - (float)((int)t);

	CVector color = m_SkyColor;
	if(getSizeX() <=4 || getSizeY() <= 4)
	{
		color.x *= m_Color.x;
		color.y *= m_Color.y;
		color.z *= m_Color.z;
	}
	else
		{CTexture::draw();}

	glColor3f(color.x, color.y, color.z);

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);

	glNormal3f(0.0, -1.0, 0.0); //not really necessary, because there's no lighting

	//Clouds
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

	//Horizon
	bool fogEnabled = glIsEnabled(GL_FOG);
	glDisable(GL_FOG);
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
	if(fogEnabled) glEnable(GL_FOG);

	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);

	glColor3f(1,1,1);
}
