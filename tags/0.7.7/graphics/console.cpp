/***************************************************************************
                          console.cpp  -  Basic text I/O (using e.g. openGL)
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
#include <cstdlib>

#include "console.h"

CFont *theConsoleFont = NULL;

CFont *loadConsoleFont()
{
	//size in pixels:
	float fontW = 10.0;
	float fontH = 20.0;

	if(theConsoleFont == NULL)
	{
		CString fontfile = "misc/iso8859-1.rgba";
		theConsoleFont = new CFont(NULL);
		printf("Loading font from %s\n", fontfile.c_str());

		CParamList plist;
		SParameter p;
		p.name = "sizex";
		p.value = 512;
		plist.push_back(p);
		p.name = "sizey";
		p.value = 512;
		plist.push_back(p);
		p.name = "smooth";
		p.value = true; //TODO?
		plist.push_back(p);
		p.name = "wth";
		p.value = fontW;
		plist.push_back(p);
		p.name = "hth";
		p.value = fontH;
		plist.push_back(p);
		if(!theConsoleFont->load(fontfile, plist))
			printf("Font loading failed!\n");
	}

	return theConsoleFont;
}

CConsole::CConsole(CWinSystem *winsys)
{
	m_WinSystem = winsys;
	loadConsoleFont();

	m_WriteMode = false;
}

CConsole::~CConsole()
{
	//TODO: find a way to unload the font
}

void CConsole::print(const CString &str)
{
	CString s = str;
	while(true)
	{
		int pos = s.inStr('\n');
		if(pos < 0)
		{
			m_ScreenContent.push_back(s);
			break;
		}
		CString lhs = s.mid(0, pos);
		s = s.mid(pos + 1);
		m_ScreenContent.push_back(lhs);
	}

	while(theConsoleFont->getFontH() * (1 + m_ScreenContent.size()) > m_WinSystem->getHeight())
		m_ScreenContent.erase(m_ScreenContent.begin());
}

void CConsole::clear()
{
	m_ScreenContent.clear();
}

void CConsole::draw()
{
	bool wasWriting = m_WriteMode;
	if(!wasWriting) enterWriteMode();

	glLoadIdentity();
	glTranslatef(0.0, m_WinSystem->getHeight() - theConsoleFont->getFontH(), 0); //set cursor
	for(unsigned int i=0; i < m_ScreenContent.size(); i++)
	{
		glPushMatrix();
		theConsoleFont->drawString(m_ScreenContent[i]);
		glPopMatrix();
		glTranslatef(0,-theConsoleFont->getFontH(),0); //next line
	}

	if(!wasWriting) leaveWriteMode();
}

bool zEnabled = true, fEnabled = true;

void CConsole::enterWriteMode()
{
	m_WriteMode = true;

	int w = m_WinSystem->getWidth();
	int h = m_WinSystem->getHeight();

	//no perspective
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1, 1);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	

	glViewport(0, 0, w, h);

	zEnabled = glIsEnabled(GL_DEPTH_TEST);
	if(zEnabled) glDisable(GL_DEPTH_TEST);
	fEnabled = glIsEnabled(GL_FOG);
	if(fEnabled) glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	theConsoleFont->enable();
}

void CConsole::leaveWriteMode()
{
	m_WriteMode = false;

	glEnable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	if(zEnabled) glEnable(GL_DEPTH_TEST);
	if(fEnabled) glEnable(GL_FOG);

	theConsoleFont->disable();
}
