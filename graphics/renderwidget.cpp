/***************************************************************************
                          renderwidget.cpp  -  GUI widget for 3D rendering
                             -------------------
    begin                : sa jul 22 2006
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
 
#include "renderwidget.h"

#include "winsystem.h"

CRenderWidget::CRenderWidget(bool enableDepth, bool enableFog, bool idleRedraw)
{
	m_EnableDepth = enableDepth;
	m_EnableFog = enableFog;
	m_IdleRedraw = idleRedraw;

	m_Renderer = NULL;
}

CRenderWidget::~CRenderWidget()
{;}

int CRenderWidget::onRedraw()
{
	CWidget::onRedraw();

	_clear3DArea(m_W, m_H);

	if(m_Renderer != NULL)
	{
		leave2DMode();

		m_Renderer->m_X = m_X;
		m_Renderer->m_Y = m_Y;
		m_Renderer->m_W = m_W;
		m_Renderer->m_H = m_H;

		m_Renderer->update();

		enter2DMode();
	}

	return 0;
}

int CRenderWidget::onIdle()
{
	if(m_IdleRedraw)
		return WIDGET_REDRAW;

	return 0;
}

void _enter2DMode(unsigned int x, unsigned int y, bool depth, bool fog)
{
	if(depth) glDisable(GL_DEPTH_TEST);
	if(fog)   glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	//glEnable(GL_SCISSOR_TEST);

	//Back to original projection:
	unsigned int w = theWinSystem->getWidth();
	unsigned int h = theWinSystem->getHeight();
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1, 1);
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glViewport(0, 0, w, h);

	glTranslatef(x, y, 0);
}

void _clear3DArea(unsigned int w, unsigned int h)
{
	//Workaround for an ugly bug in the intel i815 openGL driver for Linux
	//TODO: contact the driver maintainer and remove this workaround
	//This workaround should do approximately the same as:
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	{
		glDisable(GL_SCISSOR_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_SCISSOR_TEST);

		glColor3f(0,0,0);
		glBegin(GL_QUADS);
		glVertex2f(2,2);
		glVertex2f(w-2,2);
		glVertex2f(w-2,h-2);
		glVertex2f(2,h-2);
		glEnd();
		glColor3f(1,1,1);
	}
}

void _leave2DMode(bool depth, bool fog)
{
	glEnable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	if(depth) glEnable(GL_DEPTH_TEST);
	if(fog) glEnable(GL_FOG);

	//glDisable(GL_SCISSOR_TEST);
}

void CRenderWidget::enter2DMode()
{
	_enter2DMode(m_X, m_Y, m_EnableDepth, m_EnableFog);
}

void CRenderWidget::leave2DMode()
{
	_leave2DMode(m_EnableDepth, m_EnableFog);
}

