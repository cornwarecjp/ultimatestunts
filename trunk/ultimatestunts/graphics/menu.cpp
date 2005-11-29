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
	m_AlignLeft = false;
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

int CMenu::getdy()
{
	if(m_Lines.size() == 0) return 1;

	int dy_resizable = m_H / m_Lines.size();

	if(dy_resizable > int(theConsoleFont->getFontH())) return int(theConsoleFont->getFontH());

	return dy_resizable;
}

int CMenu::onMouseMove(int x, int y)
{
	int numl = m_Lines.size();
	int yrel = int(float(m_Y + m_H - y) / getdy());

	if(yrel < numl && yrel >= 0)
	{
		int s = yrel;
		if(s < 0) s = 0;
		if(s >= numl) s = numl-1;
		
		m_Selected = s;
		return WIDGET_REDRAW;
	}

	return 0;
}

int CMenu::onMouseClick(int x, int y, unsigned int buttons)
{
	onMouseMove(x, y);

	if(buttons == SDL_BUTTON_LEFT)
		return WIDGET_QUIT;

	return 0;
}

int CMenu::onRedraw()
{
	CWidget::onRedraw();

	if(m_Lines.size() == 0) return 0; //we're ready then

	theConsoleFont->enable();

	int dy = getdy();
	if(m_AlignLeft)
		{glTranslatef(0, m_H - dy, 0);} //to the top left point
	else
		{glTranslatef(m_W/2, m_H - dy, 0);} //to the top mid point

	//draw the list:
	for(unsigned int i=0; i < m_Lines.size(); i++)
	{
		glPushMatrix();

		//color of the selected line
		if(i == m_Selected)
			{glColor3f(1,0.9,0);}
		else
			{glColor3f(1,1,1);}

		if(!m_AlignLeft)
			{glTranslatef(-int(m_Lines[i].length())*theConsoleFont->getFontW()/2, 0, 0);} //centered

		theConsoleFont->drawString(m_Lines[i]);
		glPopMatrix();
		glTranslatef(0, -dy, 0); //next line
	}

	//back to normal:
	glColor3f(1,1,1);


	theConsoleFont->disable();

	return 0;
}
