/***************************************************************************
                          gui.cpp  -  The graphical user interface: menu's etc.
                             -------------------
    begin                : vr jan 31 2003
    copyright            : (C) 2003 by CJP
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
#include <GL/gl.h>

#include "gui.h"

CGUI::CGUI(const CLConfig &conf, CWinSystem *winsys)
{
	//TODO: get conffile options
	m_WinSys = winsys;
	m_ChildWidget = NULL;
	m_in2DMode = false;
}

CGUI::~CGUI(){
}

bool znabled, fnabled;

void CGUI::enter2DMode()
{
	if(m_in2DMode) return;
	m_in2DMode = true;

	znabled = glIsEnabled(GL_DEPTH_TEST);
	if(znabled) glDisable(GL_DEPTH_TEST);
	fnabled = glIsEnabled(GL_FOG);
	if(fnabled) glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	glEnable(GL_SCISSOR_TEST);

	int w = m_WinSys->getWidth();
	int h = m_WinSys->getHeight();
	onResize(0, 0, w, h);
}

void CGUI::leave2DMode()
{
	if(!m_in2DMode) return;
	m_in2DMode = false;

	glEnable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	if(znabled) glEnable(GL_DEPTH_TEST);
	if(fnabled) glEnable(GL_FOG);

	glDisable(GL_SCISSOR_TEST);
}

void CGUI::start()
{
	enter2DMode();

	if(m_ChildWidget != NULL)
		m_WinSys->runLoop(this);

	leave2DMode();
}

int CGUI::onMouseMove(int x, int y)
{
	return m_ChildWidget->onMouseMove(x, y);
}

int CGUI::onMouseClick(int x, int y, unsigned int buttons)
{
	return m_ChildWidget->onMouseClick(x, y, buttons);
}

int CGUI::onKeyPress(int key)
{
	return m_ChildWidget->onKeyPress(key);
}

int CGUI::onResize(int x, int y, int w, int h)
{
	CWidget::onResize(x, y, w, h);

	//no perspective
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1, 1);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glViewport(0, 0, w, h);
	glScissor(0, 0, w, h);

	return m_ChildWidget->onResize(0, 0, w, h);
}

int CGUI::onRedraw()
{
	return m_ChildWidget->onRedraw();
}
