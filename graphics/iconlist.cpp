/***************************************************************************
                          iconlist.cpp  -  List of icons for use in GUIs
                             -------------------
    begin                : za jun 9 2007
    copyright            : (C) 2007 by CJP
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

#include "iconlist.h"

CIconList *theIconList = NULL;

CIconList::CIconList()
 : CTexture(NULL)
{
	theIconList = this;

	load("misc/icons.rgb", CParamList());
}


CIconList::~CIconList()
{
	theIconList = NULL;
}

void CIconList::enable()
{
	CTexture::draw();
}

void CIconList::disable()
{
	;
}

void CIconList::draw(eIconID ID)
{
	//Calculate position in texture
	float xmin = ID & 0x7;
	float xmax = xmin + 1;
	float ymin = ID >> 3;
	float ymax = ymin + 1;

	//Scaling
	xmin /= 8.0;
	xmax /= 8.0;
	ymin /= 8.0;
	ymax /= 8.0;

	//Swap vertical coordinate
	ymin = 1.0 - ymin;
	ymax = 1.0 - ymax;

	//Draw
	glBegin(GL_QUADS);
	glTexCoord2f(xmin,ymax);
	glVertex2f(-16,-16);

	glTexCoord2f(xmax,ymax);
	glVertex2f( 16,-16);

	glTexCoord2f(xmax,ymin);
	glVertex2f( 16, 16);

	glTexCoord2f(xmin,ymin);
	glVertex2f(-16, 16);
	glEnd();
}

