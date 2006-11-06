/***************************************************************************
                          widget.cpp  -  Base-class for all GUI widgets
                             -------------------
    begin                : vr jan 7 2005
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
#include <GL/gl.h>
#include <cstdio>

#include "widget.h"

CWidget::CWidget()
{
	m_W = m_H = m_X = m_Y = 0;
	m_Xrel = m_Yrel = m_Wrel = m_Hrel = 0.0;
}

CWidget::~CWidget()
{;}

int CWidget::onMouseMove(int x, int y, unsigned int buttons)
{
	return 0;
}

int CWidget::onMouseClick(int x, int y, unsigned int buttons)
{
	return 0;
}

int CWidget::onKeyPress(int key)
{
	return 0;
}

int CWidget::onResize(int x, int y, int w, int h)
{
	m_X = x;
	m_Y = y;
	m_W = w;
	m_H = h;
	return WIDGET_REDRAW;
}

int CWidget::onRedraw()
{
	glLoadIdentity();
	glTranslatef(m_X, m_Y, 0);

	glScissor(m_X, m_Y, m_W, m_H);

	//glClear( GL_COLOR_BUFFER_BIT );

	//the rectangle
	glColor3f(0.2,0.2,0.2);
	glBindTexture(GL_TEXTURE_2D, 0); //no texture
	glBegin(GL_LINE_LOOP);
	glVertex2f(1,1);
	glVertex2f(1,m_H-1);
	glVertex2f(m_W-1,m_H-1);
	glVertex2f(m_W-1,1);
	glEnd();
	glColor3f(1,1,1);

	return 0;
}

int CWidget::onIdle()
{
	return 0;
}

void CWidget::drawBackground()
{
	glBegin(GL_QUADS);
	glVertex2f(2,2);
	glVertex2f(m_W-2,2);
	glVertex2f(m_W-2,m_H-2);
	glVertex2f(2,m_H-2);
	glEnd();
}

bool CWidget::isInWidget(int x, int y)
{
	return x >= m_X && x < m_X+m_W && y >= m_Y && y < m_Y+m_H;
}
