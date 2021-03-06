/***************************************************************************
                          font.cpp  -  A bitmap-based font class
                             -------------------
    begin                : ma okt 25 2004
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

#include <GL/gl.h>
#include <cstdio>

#include "font.h"

CFont::CFont(CDataManager *manager) : CTexture(manager)
{
	m_Texture = 0;
	m_BaseDispList = 0;
}

CFont::~CFont()
{
	unload();
}

void CFont::unload()
{
	if(!isLoaded()) return;

	//fprintf(stderr, "unloaded font\n");
	glDeleteLists(m_BaseDispList, 256);
	CTexture::unload();
}

bool CFont::load(const CString &filename, const CParamList &list)
{
	if(!CTexture::load(filename, list)) return false;

	float wth = m_ParamList.getValue("wth", "1.0").toFloat();
	float hth = m_ParamList.getValue("hth", "1.0").toFloat();
	m_W = wth;
	m_H = hth;

	m_BaseDispList = glGenLists(256);
	if(m_BaseDispList == 0) return false;
	//fprintf(stderr, "Font @ %d\n", m_BaseDispList);

	float dx = 1.0/16.0, dy = 1.0/16.0;
	for(unsigned int i=0; i < 256; i++)
	{
		float x = (float)(i % 16) / 16.0;
		float y = (float)(i - i % 16) / 256.0;

		//swapping:
		y = 1.0 - y - dy;

		glNewList(m_BaseDispList + i, GL_COMPILE);

		glBegin(GL_QUADS);
		glTexCoord2f(x,y);
		glVertex2f(0,0);

		glTexCoord2f(x+dx,y);
		glVertex2f(wth,0);

		glTexCoord2f(x+dx,y+dy);
		glVertex2f(wth,hth);

		glTexCoord2f(x,y+dy);
		glVertex2f(0,hth);

		glEnd();

		glTranslatef(wth,0,0);
		
		glEndList();
	}

	return true;
}

void CFont::enable()
{
	glPushAttrib(GL_LIST_BIT);
	glListBase(m_BaseDispList);
	CTexture::draw();
}

void CFont::disable()
{
	glPopAttrib();
}

void CFont::drawString(const CString &str)
{
	glCallLists(str.length(), GL_UNSIGNED_BYTE, (void *)(str.c_str()));
}
