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
 
#include "widget.h"

CWidget::~CWidget()
{;}

int CWidget::onMouseMove(int x, int y)
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

	//the rectangle
	glColor3f(0.2,0.2,0.2);
	glBindTexture(GL_TEXTURE_2D, 0); //no texture
	glBegin(GL_LINE_LOOP);
	glVertex2f(0,0);
	glVertex2f(0,m_H);
	glVertex2f(m_W,m_H);
	glVertex2f(m_W,0);
	glEnd();
	glColor3f(1,1,1);

	glScissor(m_X, m_Y, m_W, m_H);

	return 0;
}

bool CWidget::isInWidget(int x, int y)
{
	return x >= m_X && x < m_X+m_W && y >= m_Y && y < m_Y+m_H;
}
