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

#include <cstdio>

#include "SDL.h"
#include <GL/gl.h>

#include <libintl.h>
#define _(String) gettext (String)

#include "messagebox.h"
#include "console.h"

CMessageBox::CMessageBox()
{
	loadConsoleFont();

	m_Type = eOK;
	m_Selected = 0;
	m_Title = "Title";
	m_Cancelled = false;
}

CMessageBox::~CMessageBox(){
}

void CMessageBox::setTitle(const CString &title)
{
	m_Title = title;
	updateLines();
}

void CMessageBox::updateLines()
{
	int maxChars = int(m_W / theConsoleFont->getFontW());
	if(maxChars == 0) maxChars = 1;

	//Splitting the title up into separate lines
	m_Lines.clear();
	CString theRest = m_Title;
	while(true)
	{
		//quit when there is no need to break
		if(int(theRest.length()) <= maxChars)
		{
			m_Lines.push_back(theRest);
			break;
		}

		//find the last space before the break pos
		int breakpos = -1;
		for(int i=0; i < maxChars; i++)
		{
			if(theRest[i] == ' ') breakpos = i;
		}

		CString lhs;
		if(breakpos < 0) //if there was no space
		{
			lhs = theRest.mid(0, maxChars);
			theRest = theRest.mid(maxChars);
		}
		else //we found a space
		{
			lhs = theRest.mid(0, breakpos);
			theRest = theRest.mid(breakpos+1); //skip the space character
		}

		//printf("%d: \"%s\"\n", m_Lines.size(), lhs.c_str());
		m_Lines.push_back(lhs);
	}
}

int CMessageBox::onResize(int x, int y, int w, int h)
{
	int ret = CWidget::onResize(x, y, w, h);
	updateLines();
	return ret;
}

int CMessageBox::onKeyPress(int key)
{
	if(key == SDLK_ESCAPE) m_Cancelled = true;
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

int CMessageBox::onMouseMove(int x, int y, unsigned int buttons)
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
	onMouseMove(x, y, buttons);
	return WIDGET_QUIT;
}

int CMessageBox::onRedraw()
{
	CWidget::onRedraw();

	glColor4f(0, 0, 0, 0.8);
	drawBackground();
	glColor4f(1,1,1, 1);

	//centered
	glTranslatef(m_W/2, m_H/2, 0);

	glPushMatrix();

	switch(m_Type)
	{
		case eOK:
			glTranslatef(-theConsoleFont->getFontW(), -0.3*m_H, 0);

			glColor3f(1, 0.9, 0);
			theConsoleFont->enable();
			theConsoleFont->drawString(_("OK"));
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
			theConsoleFont->drawString(_("Yes"));
			theConsoleFont->disable();
			glPopMatrix();
			glTranslatef(0.3*m_W, -0.3*m_H, 0);

			if(m_Selected == 1)
				{glColor3f(1, 0.9, 0);}
			else
				{glColor3f(1, 1, 1);}
			theConsoleFont->enable();
			theConsoleFont->drawString(_("No"));
			theConsoleFont->disable();
			break;
	}

	glPopMatrix();

	drawTitle();

	return 0;
}

void CMessageBox::drawTitle()
{
	glPushMatrix();

	glTranslatef(0.0, 0.3*m_H, 0);
	theConsoleFont->enable();
	for(unsigned int i=0; i < m_Lines.size(); i++)
	{
		glPushMatrix();
		glTranslatef(-0.5*theConsoleFont->getFontW()*m_Lines[i].size(), -theConsoleFont->getFontH()*i, 0);

		//the title
		glColor3f(1,1,1);
		theConsoleFont->drawString(m_Lines[i]);
		glPopMatrix();
	}
	theConsoleFont->disable();

	glPopMatrix();
}
