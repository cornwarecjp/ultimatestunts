/***************************************************************************
                          texture.cpp  -  A texture class
                             -------------------
    begin                : Tue Jun 18 2002
    copyright            : (C) 2002 by CJP
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
#include "texture.h"
#include "image.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>

CTexture::CTexture()
{
	m_Color = CVector(1,0,0);
}

bool CTexture::loadFromFile(CString filename, int xs, int ys)
{
	//printf("Loading %s..\n", filename);
	RGBImageRec *in_image = RGBImageLoad(filename.c_str());

	printf("  1: %d,%d\n", xs, ys);
	RGBImageRec *image = scaleImage(in_image, xs, ys);
  if(image==NULL)
  {
    //printf("Using maximum texture size\n");
    image = in_image;
    in_image = NULL;
  }

	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexImage2D( GL_TEXTURE_2D, 0, 3, image->sizeX, image->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	if(m_GrsetTextureSmooth)
	{
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  }
	else
	{
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	}

	sizex = image->sizeX;
	sizey = image->sizeY;

  if(in_image != NULL)
  {
    free(in_image->data);
	  free(in_image);
  }
  in_image = image;

	printf("  2: %d,%d\n", xs/2, ys/2);
	image = scaleImage(in_image, xs/2, ys/2);
  if(image==NULL)
  {
    m_Texture2 = m_Texture;
  	sizex2 = sizex;
	  sizey2 = sizey;
  }
  else
  {
  	glGenTextures(1, &m_Texture2);
  	glBindTexture(GL_TEXTURE_2D, m_Texture2);
	  glTexImage2D( GL_TEXTURE_2D, 0, 3, image->sizeX, image->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

  	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

  	if(m_GrsetTextureSmooth)
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

    free(in_image->data);
	  free(in_image);
    in_image = image;
  }

	printf("  3: %d,%d\n", xs/4, ys/4);
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
  	glTexImage2D( GL_TEXTURE_2D, 0, 3, image->sizeX, image->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	  if(m_GrsetTextureSmooth)
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

    free(in_image->data);
  	free(in_image);
    in_image = image;
  }

	printf("  4: %d,%d\n", xs/8,ys/8);
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
  	glTexImage2D( GL_TEXTURE_2D, 0, 3, image->sizeX, image->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

	  if(m_GrsetTextureSmooth)
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

    free(in_image->data);
  	free(in_image);
    in_image = image;
  }

	printf("  5: 1,1\n");
	image = scaleImage(in_image, 1,1);
	if(image==NULL)
	{
		m_Color.x = (float)*(in_image->data  ) / 255;
		m_Color.y = (float)*(in_image->data+1) / 255;
		m_Color.z = (float)*(in_image->data+2) / 255;
	}
	else
	{
		m_Color.x = (float)*(image->data  ) / 255;
		m_Color.y = (float)*(image->data+1) / 255;
		m_Color.z = (float)*(image->data+2) / 255;

		free(image->data);
		free(image);
	}

	free(in_image->data);
	free(in_image);

	return true;
}

void CTexture::draw(int lod)
{
  if (getSizeX(lod) <= 4 || getSizeY(lod) <= 4)
    {printf("Error: trying to draw a too small texture\n"); return;}

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

int CTexture::getSizeX(int i)
{
	switch(i)
	{
		case 1:
			return sizex;
		case 2:
			return sizex2;
		case 3:
			return sizex3;
		case 4:
			return sizex4;
	}
  return 0;
}

int CTexture::getSizeY(int i)
{
	switch(i)
	{
		case 1:
			return sizey;
		case 2:
			return sizey2;
		case 3:
			return sizey3;
		case 4:
			return sizey4;
	}
  return 0;
}

CVector CTexture::getColor()
{
	return m_Color;
}

RGBImageRec *CTexture::scaleImage(RGBImageRec *in, int xs, int ys)
{
	int sizex,sizey;
	unsigned char *bitmapdata;
	RGBImageRec *out;

	if(xs < 1) xs = 1;
	if(ys < 1) ys = 1;

	sizex = (in->sizeX>xs)? xs : in->sizeX;
	sizey = (in->sizeY>ys)? ys : in->sizeY;

  if(sizex==in->sizeX && sizey==in->sizeY)
    return NULL; //don't scale; use the same texture


	bitmapdata = (unsigned char *)malloc(3*sizex*sizey*sizeof(unsigned char));
	out = (RGBImageRec *)malloc(sizeof(RGBImageRec));

	out->data = bitmapdata;
	out->sizeX = sizex;
	out->sizeY = sizey;

#undef USE_GLUSCALE
//I guess gluscaleImage is faster than my own algoritm,
//But it gives poor textures at low-res.

#ifdef USE_GLUSCALE
  if(sizex >= 4 && sizey >= 4)
  {
    gluScaleImage(GL_RGB,
      in->sizeX,  in->sizeY,  GL_UNSIGNED_BYTE, in->data,
      out->sizeX, out->sizeY, GL_UNSIGNED_BYTE, out->data );
  }
  else //still need my own algoritm for very small sizes
  {
#endif
  	int bsx;
	  int bsy;
  	int opp=1;
	  int i, j, x, y;
  	int r, g, b;

	  bsx = in->sizeX / xs;
  	bsy = in->sizeY / ys;

	  if(bsx<1) {bsx=1;}
  	if(bsy<1) {bsy=1;}
	  opp = bsx*bsy;

  	//printf("Oorspronkelijke resolutie: %dx%d\n",in->sizeX, in->sizeY);
	  //printf("Resizing with blocksize %dx%d...\n",bsx,bsy);

  	for(x=0; x<sizex; x++)
	  	for(y=0; y<sizey; y++)
		  {
			  r = g = b = 0;
  			for(i=0; i<bsx; i++)
	  			for(j=0; j<bsy; j++)
		  		{
			  		r += *(in->data + 3*(bsx*x+i+in->sizeX*(bsy*y+j)) );
				  	g += *(in->data + 3*(bsx*x+i+in->sizeX*(bsy*y+j)) +1);
					  b += *(in->data + 3*(bsx*x+i+in->sizeX*(bsy*y+j)) +2);
  				}

	  			*(bitmapdata + 3*(x+sizex*y) ) = r/opp;
		  		*(bitmapdata + 3*(x+sizex*y) +1) = g/opp;
			  	*(bitmapdata + 3*(x+sizex*y) +2) = b/opp;
  		}
#ifdef USE_GLUSCALE
  }
#endif

	return out;
}
