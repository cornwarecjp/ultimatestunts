/***************************************************************************
                          compass.cpp  -  A compass for navigating through the world
                             -------------------
    begin                : ma sep 24 2007
    copyright            : (C) 2007 by CJP
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
#include <cstdlib>

#include <GL/gl.h>

#include "renderwidget.h"
#include "compass.h"

CCompass::CCompass()
{
	m_Camera = NULL;
	m_Selection = eNone;
}


CCompass::~CCompass()
{
}

int CCompass::onMouseMove(int x, int y, unsigned int buttons)
{
	CVector curpos(x - CWidget::m_W/2, y - CWidget::m_H/2, 0.0);

	curpos *= m_Camera->getOrientation();

	eSelection newSel = eNone;
	if(curpos.x > curpos.z && curpos.x > -curpos.z)
		{newSel = eXup;}
	else if(curpos.x < curpos.z && curpos.x < -curpos.z)
		{newSel = eXdn;}
	else if(curpos.z > curpos.x && curpos.z > -curpos.x)
		{newSel = eZup;}
	else if(curpos.z < curpos.x && curpos.z < -curpos.x)
		{newSel = eZdn;}

	if(x > 0.8*CWidget::m_W)
	{
		if(y > CWidget::m_H/2)
			{newSel = eYup;}
		else
			{newSel = eYdn;}
	}

	if(newSel != m_Selection)
	{
		m_Selection = newSel;
		return WIDGET_REDRAW;
	}

	return 0;
}

int CCompass::onMouseClick(int x, int y, unsigned int buttons)
{
	int ret = onMouseMove(x, y, buttons);

	if(m_Selection != eNone)
		ret |= WIDGET_QUIT;

	return ret;
}

int CCompass::onRedraw()
{
	CWidget::onRedraw();

	_clear3DArea(CWidget::m_W, CWidget::m_H);

	drawUpDown();

	_leave2DMode(true, false);
	CRenderer::update(); //updates viewport etc.
	drawCompass();
	_enter2DMode(CWidget::m_X, CWidget::m_Y, true, false);

	return 0;
}

void CCompass::updateScreenSize()
{
	CRenderer::m_X = CWidget::m_X;
	CRenderer::m_Y = CWidget::m_Y;
	CRenderer::m_W = CWidget::m_W;
	CRenderer::m_H = CWidget::m_H;
}

void CCompass::setColor(bool selected)
{
	if(selected)
	{
		glColor3f(1,0.9,0);
		glLineWidth(2.0);
	}
	else
	{
		glColor3f(1,1,1);
		glLineWidth(1.0);
	}
}

void CCompass::drawCompass()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	//Set the camera
	const CMatrix &cammat = m_Camera->getOrientation();

	glTranslatef(0,0,-10);
	glMultMatrixf(cammat.transpose().gl_mtr());

	//X+
	setColor(m_Selection == eXup);
	glBegin(GL_LINES);
	glVertex3f(0  , 0, 0  );
	glVertex3f(2  , 0, 0  );
	glVertex3f(2  , 0, 0  );
	glVertex3f(1.5, 0,-0.5);
	glVertex3f(2  , 0, 0  );
	glVertex3f(1.5, 0, 0.5);
	glEnd();

	//X-
	setColor(m_Selection == eXdn);
	glBegin(GL_LINES);
	glVertex3f( 0  , 0, 0  );
	glVertex3f(-2  , 0, 0  );
	glVertex3f(-2  , 0, 0  );
	glVertex3f(-1.5, 0,-0.5);
	glVertex3f(-2  , 0, 0  );
	glVertex3f(-1.5, 0, 0.5);
	glEnd();

	//Z+
	setColor(m_Selection == eZup);
	glBegin(GL_LINES);
	glVertex3f( 0  , 0, 0  );
	glVertex3f( 0  , 0, 2  );
	glVertex3f( 0  , 0, 2  );
	glVertex3f(-0.5, 0, 1.5);
	glVertex3f( 0  , 0, 2  );
	glVertex3f( 0.5, 0, 1.5);
	glEnd();

	//Z-
	setColor(m_Selection == eZdn);
	glBegin(GL_LINES);
	glVertex3f( 0  , 0, 0  );
	glVertex3f( 0  , 0,-2  );
	glVertex3f( 0  , 0,-2  );
	glVertex3f(-0.5, 0,-1.5);
	glVertex3f( 0  , 0,-2  );
	glVertex3f( 0.5, 0,-1.5);
	glEnd();


	glLineWidth(1.0);
	glEnable(GL_TEXTURE_2D);
}

void CCompass::drawUpDown()
{
	glPushMatrix();
	glTranslatef(0.9*CWidget::m_W, 0.5*CWidget::m_H, 0.0);
	glScalef(0.5*CWidget::m_H, 0.5*CWidget::m_H, 1.0);

	setColor(m_Selection == eYup);
	glBegin(GL_LINES);
	glVertex3f( 0  , 0.1, 0);
	glVertex3f( 0  , 0.9, 0);
	glVertex3f( 0  , 0.9, 0);
	glVertex3f(-0.2, 0.7, 0);
	glVertex3f( 0  , 0.9, 0);
	glVertex3f( 0.2, 0.7, 0);
	glEnd();

	setColor(m_Selection == eYdn);
	glBegin(GL_LINES);
	glVertex3f( 0  ,-0.1, 0);
	glVertex3f( 0  ,-0.9, 0);
	glVertex3f( 0  ,-0.9, 0);
	glVertex3f(-0.2,-0.7, 0);
	glVertex3f( 0  ,-0.9, 0);
	glVertex3f( 0.2,-0.7, 0);
	glEnd();

	glPopMatrix();

	glLineWidth(1.0);
}

