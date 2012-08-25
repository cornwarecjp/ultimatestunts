/***************************************************************************
                          longmenu.cpp  -  menu with scrolling ability
                             -------------------
    begin                : ma jan 2 2006
    copyright            : (C) 2006 by CJP
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

#include "SDL.h"

#include "longmenu.h"
#include "console.h"

CLongMenu::CLongMenu()
{
}

CLongMenu::~CLongMenu(){
}

int CLongMenu::onKeyPress(int key)
{
	//No top <=> movements
	if(key == SDLK_UP && m_Selected == 0)
		return 0;
	if(key == SDLK_DOWN && m_Selected == m_Lines.size()-1)
		return 0;

	return CMenu::onKeyPress(key);
}

int CLongMenu::onMouseMove(int x, int y, unsigned int buttons)
{
	//Scrollbar handling
	if(isInScrollbar(x, y))
	{
		if(handleScrollbarMove(x, y, buttons))
		{
			unsigned int screentop = getScreenTop();
			unsigned int numLines = getNumVisibleLines();

			if(m_Selected < screentop)
				m_Selected = screentop;
			if(m_Selected > screentop+numLines-1)
				m_Selected = screentop+numLines-1;

			return WIDGET_REDRAW;
		}

		return 0;
	}

	int ret = CMenu::onMouseMove(x, y, buttons);

	unsigned int screentop = getScreenTop();
	unsigned int numLines = getNumVisibleLines();

	if(m_Selected == 0 && screentop > 0)
		{m_Selected += screentop - 1;} //scroll up
	else if(m_Selected == numLines-1 && screentop + numLines <= m_Lines.size())
		{m_Selected += screentop + 1;} //scroll down
	else
		{m_Selected += screentop;}

	if(m_Selected >= m_Lines.size())
		m_Selected = m_Lines.size() -1;

	return ret;
}

int CLongMenu::onMouseClick(int x, int y, unsigned int buttons)
{
	if(isInScrollbar(x,y))
		return 0;

	int ret = CMenu::onMouseClick(x, y, buttons);

	m_Selected += getScreenTop();
	if(m_Selected >= m_Lines.size())
		m_Selected = m_Lines.size() -1;

	return ret;
}

int CLongMenu::onRedraw()
{
	m_RequestH = (1+m_Lines.size()) * int(theConsoleFont->getFontH());

	unsigned int numLines = getNumVisibleLines();
	unsigned int screentop = getScreenTop();

	//Update scroll position if necessary
	if(m_Selected < screentop)
	{
		screentop = m_Selected;
		m_ScrollPosition = screentop * int(theConsoleFont->getFontH());
	}
	else if(m_Selected > screentop + numLines - 1)
	{
		screentop = m_Selected - numLines + 1;
		m_ScrollPosition = screentop * int(theConsoleFont->getFontH());
	}

	//Backup
	vector<CString> lines_bak = m_Lines;
	unsigned int selected_bak = m_Selected;

	//Modify
	m_Lines.clear();
	for(unsigned int i= screentop; i < screentop+numLines; i++)
	{
		if(i >= lines_bak.size()) break;
		m_Lines.push_back(lines_bak[i]);
	}

	m_Selected -= screentop;

	//Draw
	int ret = CMenu::onRedraw();

	//restore
	m_Lines = lines_bak;
	m_Selected = selected_bak;


	drawScrollbar();

	return ret;
}

unsigned int CLongMenu::getNumVisibleLines()
{
	return (unsigned int) m_H / int(theConsoleFont->getFontH());
}

unsigned int CLongMenu::getScreenTop()
{
	return (unsigned int) m_ScrollPosition / int(theConsoleFont->getFontH());
}

int CLongMenu::getdy()
{
	unsigned int numLines = getNumVisibleLines();

	if(m_Lines.size() < numLines) return CMenu::getdy();

	return m_H / numLines;
}
