/***************************************************************************
                          guipage.cpp  -  A page of a menu interface
                             -------------------
    begin                : wo dec 15 2004
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
#include "SDL.h"

#include "guipage.h"
#include "console.h"
#include "texture.h"

CTexture *_thePageBackground = NULL;

CGUIPage::CGUIPage()
{
	loadConsoleFont();

	m_Title = "Ultimate Stunts menu";

	if(_thePageBackground == NULL)
	{
		CParamList plist;
		SParameter p;
		p.name = "sizex";
		p.value = 4096; //at maximum of course :-)
		plist.push_back(p);
		p.name = "sizey";
		p.value = 4096;
		plist.push_back(p);
		_thePageBackground = new CTexture(NULL);
		_thePageBackground->load("misc/menubackground.rgb", plist);
	}
}

CGUIPage::~CGUIPage(){
}

int CGUIPage::onKeyPress(int key)
{
	return m_Menu.onKeyPress(key);
}

int CGUIPage::onRedraw()
{
	//draw background:
	glLoadIdentity();
	_thePageBackground->draw();
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2f(0,0);
	glTexCoord2f(1,0);
	glVertex2f(m_W,0);
	glTexCoord2f(1,1);
	glVertex2f(m_W,m_H);
	glTexCoord2f(0,1);
	glVertex2f(0,m_H);
	glEnd();

	theConsoleFont->enable();

	//draw the title:
	glLoadIdentity();
	glTranslatef(m_W/2, m_H/2 + ((int)m_Menu.m_Lines.size() + 4)*theConsoleFont->getFontH()/2, 0); //set cursor

	glPushMatrix();

	glColor3f(1,1,1);

	glTranslatef(-((int)m_Title.length())*theConsoleFont->getFontW(), 0, 0); //centered
	glScalef(2,2,2);
	theConsoleFont->drawString(m_Title);
	glPopMatrix();

	theConsoleFont->disable();

	m_Menu.onRedraw();

	return 0;
}

int CGUIPage::onResize(int w, int h)
{
	return CWidget::onResize(w, h) | m_Menu.onResize(w, h);
}
