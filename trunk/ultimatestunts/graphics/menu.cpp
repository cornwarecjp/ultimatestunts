/***************************************************************************
                          menu.cpp  -  A menu GUI element
                             -------------------
    begin                : do dec 16 2004
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

#include "menu.h"
#include "console.h"

CMenu::CMenu()
{
	loadConsoleFont();
	m_Selected = 0;
}

CMenu::~CMenu(){
}

int CMenu::onKeyPress(int key)
{
	if(key == SDLK_DOWN)
		if(m_Selected < m_Lines.size()-1)
			{m_Selected++;}
		else
			{m_Selected = 0;}

	if(key == SDLK_UP)
		if(m_Selected > 0)
			{m_Selected--;}
		else
			{m_Selected = m_Lines.size()-1;}

	if(key == SDLK_RETURN) return WIDGET_QUIT;
	if(key == SDLK_ESCAPE) return WIDGET_CANCELLED | WIDGET_QUIT;

	return WIDGET_REDRAW;
}

int CMenu::onRedraw()
{
	theConsoleFont->enable();

	glLoadIdentity();
	glTranslatef(m_W/2, m_H/2 + ((int)m_Lines.size())*theConsoleFont->getFontH()/2, 0); //set cursor

	//draw the list:
	for(unsigned int i=0; i < m_Lines.size(); i++)
	{
		glPushMatrix();

		//color of the selected line
		if(i == m_Selected)
			{glColor3f(1,0.9,0);}
		else
			{glColor3f(1,1,1);}

		glTranslatef(-((int)m_Lines[i].length())*theConsoleFont->getFontW()/2, 0, 0); //centered
		theConsoleFont->drawString(m_Lines[i]);
		glPopMatrix();
		glTranslatef(0, -theConsoleFont->getFontH(), 0); //next line
	}

	//back to normal:
	glColor3f(1,1,1);


	theConsoleFont->disable();
	return 0;
}
