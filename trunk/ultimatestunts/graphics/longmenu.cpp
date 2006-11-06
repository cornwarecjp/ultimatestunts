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

#include "SDL.h"

#include "longmenu.h"
#include "console.h"

CLongMenu::CLongMenu()
{
	m_ScreenTop = 0;
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
	int ret = CMenu::onMouseMove(x, y, buttons);

	if(m_Selected == 0 && m_ScreenTop > 0)
		{m_Selected += m_ScreenTop - 1;} //scroll up
	else if(m_Selected == getNumVisibleLines()-1 && m_ScreenTop + getNumVisibleLines() <= m_Lines.size())
		{m_Selected += m_ScreenTop + 1;} //scroll down
	else
		{m_Selected += m_ScreenTop;}

	if(m_Selected >= m_Lines.size())
		m_Selected = m_Lines.size() -1;

	return ret;
}

int CLongMenu::onMouseClick(int x, int y, unsigned int buttons)
{
	int ret = CMenu::onMouseClick(x, y, buttons);

	m_Selected += m_ScreenTop;
	if(m_Selected >= m_Lines.size())
		m_Selected = m_Lines.size() -1;

	return ret;
}

int CLongMenu::onRedraw()
{
	unsigned int numLines = getNumVisibleLines();

	//Update scroll position if necessary
	if(m_Selected < m_ScreenTop)
		{m_ScreenTop = m_Selected;}
	else if(m_Selected > m_ScreenTop + numLines - 1)
		{m_ScreenTop = m_Selected - numLines + 1;}

	//Backup
	vector<CString> lines_bak = m_Lines;
	unsigned int selected_bak = m_Selected;

	//Modify
	m_Lines.clear();
	for(unsigned int i= m_ScreenTop; i < m_ScreenTop+numLines; i++)
	{
		if(i >= lines_bak.size()) break;
		m_Lines.push_back(lines_bak[i]);
	}

	m_Selected -= m_ScreenTop;

	//Draw
	int ret = CMenu::onRedraw();

	//restore
	m_Lines = lines_bak;
	m_Selected = selected_bak;

	return ret;
}

unsigned int CLongMenu::getNumVisibleLines()
{
	return (unsigned int) m_H / int(theConsoleFont->getFontH());
}

int CLongMenu::getdy()
{
	unsigned int numLines = getNumVisibleLines();

	if(m_Lines.size() < numLines) return CMenu::getdy();

	return m_H / numLines;
}
