/***************************************************************************
                          listbox.cpp  -  A List Box
                             -------------------
    begin                : za mrt 8 2008
    copyright            : (C) 2008 by CJP
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

#include "console.h"
#include "listbox.h"

CListBox::CListBox()
{
	loadConsoleFont();

	m_Title = "Title";
	m_Cancelled = false;
}


CListBox::~CListBox()
{
}

void CListBox::setTitle(const CString &title)
{
	m_Title = title;
	updateLines();
}

void CListBox::setOptions(const vector<CString> &options)
{
	m_Menu.m_Lines = options;
}

void CListBox::setSelected(unsigned int sel)
{
	m_Menu.m_Selected = sel;
}

unsigned int CListBox::getSelected() const
{
	return m_Menu.m_Selected;
}

void CListBox::updateLines()
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

int CListBox::onResize(int x, int y, int w, int h)
{
	int ret = CWidget::onResize(x, y, w, h);
	updateLines();

	ret = ret | m_Menu.onResize(int(x + 0.1*w), int(y + 0.2*h), int(0.8*w), int(0.5*h));

	return ret;
}

int CListBox::onKeyPress(int key)
{
	int ret = m_Menu.onKeyPress(key);

	if(ret & WIDGET_CANCELLED) m_Cancelled = true;

	return ret;
}

int CListBox::onMouseClick(int x, int y, unsigned int buttons)
{
	if(m_Menu.isInWidget(x, y))
	{
		int ret = m_Menu.onMouseClick(x, y, buttons);
		if(ret & WIDGET_CANCELLED) m_Cancelled = true;
		return ret;
	}

	if(y < m_Menu.getY())
	{
		m_Cancelled = true;
		return WIDGET_CANCELLED | WIDGET_QUIT;
	}

	return 0;
}

int CListBox::onMouseMove(int x, int y, unsigned int buttons)
{
	if(m_Menu.isInWidget(x, y))
		return m_Menu.onMouseMove(x, y, buttons);

	return 0;
}

int CListBox::onRedraw()
{
	CWidget::onRedraw();

	glColor4f(0, 0, 0, 0.8);
	drawBackground();
	glColor4f(1,1,1, 1);

	//centered
	glTranslatef(m_W/2, m_H/2, 0);

	glPushMatrix();

	glTranslatef(-3*theConsoleFont->getFontW(), -0.4*m_H, 0);

	glColor3f(1, 0.9, 0);
	theConsoleFont->enable();
	theConsoleFont->drawString(_("Cancel"));
	theConsoleFont->disable();

	glPopMatrix();

	drawTitle();

	return m_Menu.onRedraw();
}

void CListBox::drawTitle()
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

