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

CGUIPage::~CGUIPage()
{
	for(unsigned int i=0; i < m_Widgets.size(); i++)
		delete m_Widgets[i];
}

int CGUIPage::onKeyPress(int key)
{
	if(m_Widgets.size() == 0) return 0;

	return m_Widgets[m_Widgets.size()-1]->onKeyPress(key);
}

int CGUIPage::onMouseMove(int x, int y)
{
	if(m_Widgets.size() == 0) return 0;

	if(m_Widgets[m_Widgets.size()-1]->isInWidget(x, y))
		return m_Widgets[m_Widgets.size()-1]->onMouseMove(x, y);

	return 0;
}

int CGUIPage::onMouseClick(int x, int y, unsigned int buttons)
{
	if(m_Widgets.size() == 0) return 0;

	if(m_Widgets[m_Widgets.size()-1]->isInWidget(x, y))
		return m_Widgets[m_Widgets.size()-1]->onMouseClick(x, y, buttons);

	return 0;
}

int CGUIPage::onRedraw()
{
	CWidget::onRedraw();

	//draw background:
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
	glTranslatef(0.5*m_W, 0.8*m_H, 0); //set cursor

	glColor3f(1,1,1);

	glTranslatef(-((int)m_Title.length())*theConsoleFont->getFontW(), 0, 0); //centered
	glScalef(2,2,2);
	theConsoleFont->drawString(m_Title);

	theConsoleFont->disable();

	if(m_Widgets.size() == 0) return 0;

	for(unsigned int i=0; i < m_Widgets.size(); i++)
		m_Widgets[i]->onRedraw();

	return 0;
}

int CGUIPage::onResize(int x, int y, int w, int h)
{
	int ret = CWidget::onResize(x, y, w, h);
	
	if(m_Widgets.size() == 0) return ret;

	for(unsigned int i=0; i < m_Widgets.size(); i++)
		ret |= m_Widgets[i]->onResize(
			int(x+m_Widgets[i]->m_Xrel*w),
			int(y+m_Widgets[i]->m_Yrel*h),
			int(m_Widgets[i]->m_Wrel*w),
			int(m_Widgets[i]->m_Hrel*h)
			);

	return ret;
}
