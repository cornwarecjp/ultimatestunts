/***************************************************************************
                          messagebox.cpp  -  A Message Box
                             -------------------
    begin                : wo jan 12 2005
    copyright            : (C) 2005 by CJP
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

#include "SDL.h"
#include <GL/gl.h>

#include "messagebox.h"
#include "console.h"

CMessageBox::CMessageBox()
{
	loadConsoleFont();

	m_Type = eOK;
	m_Selected = 0;
	m_Title = "Title";
}

CMessageBox::~CMessageBox(){
}

int CMessageBox::onKeyPress(int key)
{
	if(key == SDLK_RETURN || key == SDLK_ESCAPE) return WIDGET_QUIT;

	switch(m_Type)
	{
		case eOK:
			break;
		case eYesNo:
			switch(key)
			{
				case SDLK_TAB:
					m_Selected = 1 - m_Selected;
					break;
				case SDLK_RIGHT:
					m_Selected = 1;
					break;
				case SDLK_LEFT:
					m_Selected = 0;
					break;
			}
			break;
	}

	return WIDGET_REDRAW;
}

int CMessageBox::onMouseMove(int x, int y)
{
	switch(m_Type)
	{
		case eOK:
			return 0;
		case eYesNo:
		
			if(y - m_Y < m_H/2)
			{
				if(x - m_X < m_W/2)
					{m_Selected = 0;}
				else
					{m_Selected = 1;}

				return WIDGET_REDRAW;
			}

	}

	return 0;
}

int CMessageBox::onMouseClick(int x, int y, unsigned int buttons)
{
	onMouseMove(x, y);
	return WIDGET_QUIT;
}

int CMessageBox::onRedraw()
{
	CWidget::onRedraw();

	glColor4f(0, 0, 0, 0.7);
	drawBackground();
	glColor3f(1,1,1);

	//centered
	glTranslatef(m_W/2, m_H/2, 0);

	glPushMatrix();

	switch(m_Type)
	{
		case eOK:
			glTranslatef(-theConsoleFont->getFontW(), -0.3*m_H, 0);

			glColor3f(1, 0.9, 0);
			theConsoleFont->enable();
			theConsoleFont->drawString("OK");
			theConsoleFont->disable();
			break;

		case eYesNo:
			glPushMatrix();
			glTranslatef(-0.3*m_W, -0.3*m_H, 0);

			if(m_Selected == 0)
				{glColor3f(1, 0.9, 0);}
			else
				{glColor3f(1, 1, 1);}
			theConsoleFont->enable();
			theConsoleFont->drawString("Yes");
			theConsoleFont->disable();
			glPopMatrix();
			glTranslatef(0.3*m_W, -0.3*m_H, 0);

			if(m_Selected == 1)
				{glColor3f(1, 0.9, 0);}
			else
				{glColor3f(1, 1, 1);}
			theConsoleFont->enable();
			theConsoleFont->drawString("No");
			theConsoleFont->disable();
			break;
	}

	glPopMatrix();

	glTranslatef(-0.5*theConsoleFont->getFontW()*m_Title.size(), 0.3*m_H, 0);

	//the title
	glColor3f(1,1,1);
	theConsoleFont->enable();
	theConsoleFont->drawString(m_Title);
	theConsoleFont->disable();

	return 0;
}
