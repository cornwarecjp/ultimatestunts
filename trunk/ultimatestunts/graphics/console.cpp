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

#include "SDL.h"
#include <GL/gl.h>

#include <cstdio>
#include <cstdlib>

#include "console.h"
#include "datafile.h"

CConsole::CConsole(CWinSystem *winsys)
{
	m_WinSystem = winsys;

	//size in pixels:
	m_fontW = 12.0;
	m_fontH = 24.0;

	CDataFile df("misc/courier.rgba");
	printf("Loading font from %s\n", df.useExtern().c_str());
	if(!m_Font.loadFromFile(df.useExtern(), 512, m_fontW, m_fontH))
		printf("Font loading failed!\n");

	m_WriteMode = false;
}

CConsole::~CConsole()
{
	m_Font.unload();
}

void CConsole::print(const CString &str)
{
	bool wasWriting = m_WriteMode;
	if(!wasWriting) enterWriteMode();

	printf(str.c_str());

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

	while(m_fontH * (1 + m_ScreenContent.size()) > m_WinSystem->getHeight())
		m_ScreenContent.erase(m_ScreenContent.begin());

	clearScreen();
	draw();

	if(!wasWriting) leaveWriteMode();
}

CString CConsole::getInput(const CString &question)
{
	bool wasWriting = m_WriteMode;
	if(!wasWriting) enterWriteMode();

	print(question);

	m_WinSystem->swapBuffers();

	char input[80];
	fgets(input, 79, stdin);
	CString ret(input);
	ret = ret.mid(0, ret.length() - 1); //remove \n

	print(ret);

	if(!wasWriting) leaveWriteMode();

	return ret;
}

void CConsole::clear()
{
	m_ScreenContent.clear();
}

void CConsole::clearScreen()
{
	bool wasWriting = m_WriteMode;
	if(!wasWriting) enterWriteMode();

	float oldClear[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, oldClear);
	glClearColor(0,0.1,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(oldClear[0],oldClear[1],oldClear[2],oldClear[3]);

	if(!wasWriting) leaveWriteMode();
}

void CConsole::draw()
{
	bool wasWriting = m_WriteMode;
	if(!wasWriting) enterWriteMode();

	glTranslatef(0.0, m_WinSystem->getHeight() - m_fontH, 0); //set cursor
	for(unsigned int i=0; i < m_ScreenContent.size(); i++)
	{
		glPushMatrix();
		m_Font.drawString(m_ScreenContent[i]);
		glPopMatrix();
		glTranslatef(0,-m_fontH,0); //next line
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

	m_Font.enable();
}

void CConsole::leaveWriteMode()
{
	m_WriteMode = false;

	glEnable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	if(zEnabled) glEnable(GL_DEPTH_TEST);
	if(fEnabled) glEnable(GL_FOG);

	m_Font.disable();
}

