/***************************************************************************
                          lodtexture.cpp  -  Texture with multiple Levels Of Detail
                             -------------------
    begin                : do okt 28 2004
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

#include <cstdio>
#include <GL/gl.h>

#include "lodtexture.h"

CLODTexture::CLODTexture(CDataManager *manager) : CTexture(manager)
{
	m_Texture2 = m_Texture3 = m_Texture4 = 0;
}

CLODTexture::~CLODTexture()
{
}

void CLODTexture::unload()
{
	if(!isLoaded()) return;

	CTexture::unload();
	glDeleteTextures(1, &m_Texture2);
	glDeleteTextures(1, &m_Texture3);
	glDeleteTextures(1, &m_Texture4);
	m_Texture2 = m_Texture3 = m_Texture4 = 0;
}

RGBImageRec *CLODTexture::loadFromImage(RGBImageRec *in_image, int xs, int ys)
{
	in_image = CTexture::loadFromImage(in_image, xs, ys);

	RGBImageRec *image = scaleImage(in_image, xs/2, ys/2);
	if(image==NULL)
	{
		m_Texture2 = m_Texture;
		sizex2 = m_Sizex;
		sizey2 = m_Sizey;
	}
	else
	{
		glGenTextures(1, &m_Texture2);
		glBindTexture(GL_TEXTURE_2D, m_Texture2);

		if(image->format == 1) //RGBA
			{glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, image->sizeX, image->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);}
		else //Assume RGB
			{glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8,  image->sizeX, image->sizeY, 0, GL_RGB,  GL_UNSIGNED_BYTE, image->data);}

		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

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

		sizex2 = image->sizeX;
		sizey2 = image->sizeY;

		freeImage(in_image);
		in_image = image;
	}

	image = scaleImage(in_image, xs/4,ys/4);
	if(image==NULL)
	{
		m_Texture3 = m_Texture2;
		sizex3 = sizex2;
		sizey3 = sizey2;
	}
	else
	{
		glGenTextures(1, &m_Texture3);
		glBindTexture(GL_TEXTURE_2D, m_Texture3);

		if(image->format == 1) //RGBA
			{glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, image->sizeX, image->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);}
		else //Assume RGB
			{glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8,  image->sizeX, image->sizeY, 0, GL_RGB,  GL_UNSIGNED_BYTE, image->data);}

		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

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

		sizex3 = image->sizeX;
		sizey3 = image->sizeY;

		freeImage(in_image);
		in_image = image;
	}

	image = scaleImage(in_image, xs/8,ys/8);
	if(image==NULL)
	{
		m_Texture4 = m_Texture3;
		sizex4 = sizex3;
		sizey4 = sizey3;
	}
	else
	{
		glGenTextures(1, &m_Texture4);
		glBindTexture(GL_TEXTURE_2D, m_Texture4);

		if(image->format == 1) //RGBA
			{glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, image->sizeX, image->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->data);}
		else //Assume RGB
			{glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8,  image->sizeX, image->sizeY, 0, GL_RGB,  GL_UNSIGNED_BYTE, image->data);}

		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

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

		sizex4 = image->sizeX;
		sizey4 = image->sizeY;

		freeImage(in_image);
		in_image = image;
	}

	return in_image;
}

int CLODTexture::getSizeX(int i) const
{
	switch(i)
	{
		case 1:
			return m_Sizex;
		case 2:
			return sizex2;
		case 3:
			return sizex3;
		case 4:
			return sizex4;
	}
  return 0;
}

int CLODTexture::getSizeY(int i) const
{
	switch(i)
	{
		case 1:
			return m_Sizey;
		case 2:
			return sizey2;
		case 3:
			return sizey3;
		case 4:
			return sizey4;
	}
  return 0;
}

void CLODTexture::draw(int lod) const
{
	if (getSizeX(lod) <= 4 || getSizeY(lod) <= 4)
		{printf("   Error: trying to draw a too small texture:\n"
		"   lod=%d, x=%d, y=%d\n", lod, getSizeX(lod), getSizeY(lod)); return;}

	switch(lod)
	{
	case 1:
		glBindTexture(GL_TEXTURE_2D, m_Texture );
		break;
	case 2:
		glBindTexture(GL_TEXTURE_2D, m_Texture2);
		break;
	case 3:
		glBindTexture(GL_TEXTURE_2D, m_Texture3);
		break;
	case 4:
		glBindTexture(GL_TEXTURE_2D, m_Texture4);
	}
}

unsigned int CLODTexture::getTextureID(int lod) const
{
	switch(lod)
	{
	case 1:
		return m_Texture;
		break;
	case 2:
		return m_Texture2;
		break;
	case 3:
		return m_Texture3;
		break;
	case 4:
		return m_Texture4;
	}

	return 0;
}
