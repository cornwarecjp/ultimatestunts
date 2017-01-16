/***************************************************************************
                          colorselect.cpp  -  A Color Selection tool
                             -------------------
    begin                : di sep 05 2006
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

#include <cstdio>

#include "SDL.h"
#include <GL/gl.h>

#include <libintl.h>
#define _(String) gettext (String)

#include "colorselect.h"
#include "console.h"

CColorSelect::CColorSelect()
{
	loadConsoleFont();

	m_Cancelled = false;
	m_Title = "Title";
	m_Selected = eNone;
}

CColorSelect::~CColorSelect(){
}

void CColorSelect::setTitle(const CString &title)
{
	m_Title = title;
}

int CColorSelect::onKeyPress(int key)
{
	if(key == SDLK_ESCAPE) m_Cancelled = true;

	if(key == SDLK_RETURN || key == SDLK_ESCAPE) return WIDGET_QUIT;

	return WIDGET_REDRAW;
}

int CColorSelect::onMouseMove(int x, int y, unsigned int buttons)
{
	if(y - m_Y < 0.3*m_H)
	{
		if(x - m_X < m_W/2)
			{m_Selected = eOK;}
		else
			{m_Selected = eCancel;}
	}
	else
	{
		m_Selected = eNone;

		if(buttons & SDL_BUTTON(1))
		{
			float yrel = y - m_Y - m_H/2;
			float value = (x-m_X - 0.1*m_W) / (0.8*m_W);

			if(yrel > 0.125*m_H)
				{m_Color.x = value;}
			else if(yrel < -0.025*m_H)
				{m_Color.z = value;}
			else
				{m_Color.y = value;}

			if(m_Color.x > 1.0) m_Color.x = 1.0;
			if(m_Color.y > 1.0) m_Color.y = 1.0;
			if(m_Color.z > 1.0) m_Color.z = 1.0;
			if(m_Color.x < 0.0) m_Color.x = 0.0;
			if(m_Color.y < 0.0) m_Color.y = 0.0;
			if(m_Color.z < 0.0) m_Color.z = 0.0;
		}
	}

	return WIDGET_REDRAW;
}

int CColorSelect::onMouseClick(int x, int y, unsigned int buttons)
{
	onMouseMove(x, y, buttons);

	if(m_Selected == eOK)
		{m_Cancelled = false; return WIDGET_QUIT;}
	if(m_Selected == eCancel)
		{m_Cancelled = true; return WIDGET_QUIT;}

	return 0;
}

int CColorSelect::onRedraw()
{
	CWidget::onRedraw();

	glColor4f(0, 0, 0, 0.7);
	drawBackground();
	glColor3f(1,1,1);

	//centered
	glTranslatef(m_W/2, m_H/2, 0);

	//A colored square
	glPushMatrix();
	glTranslatef(0.0*m_W, -0.3*m_H, 0);
	glColor3f(m_Color.x, m_Color.y, m_Color.z);
	glBegin(GL_QUADS);
	glVertex2f(-0.1*m_W, -0.1*m_H);
	glVertex2f( 0.1*m_W, -0.1*m_H);
	glVertex2f( 0.1*m_W,  0.1*m_H);
	glVertex2f(-0.1*m_W,  0.1*m_H);
	glEnd();
	glPopMatrix();

	//The three color slides
	glBegin(GL_QUADS);

	glColor3f(0.0, m_Color.y, m_Color.z);
	glVertex2f(-0.4*m_W, 0.2*m_H +5);
	glVertex2f(-0.4*m_W, 0.2*m_H -5);
	glColor3f(1.0, m_Color.y, m_Color.z);
	glVertex2f( 0.4*m_W, 0.2*m_H -5);
	glVertex2f( 0.4*m_W, 0.2*m_H +5);

	glColor3f(m_Color.x, 0.0, m_Color.z);
	glVertex2f(-0.4*m_W, 0.05*m_H +5);
	glVertex2f(-0.4*m_W, 0.05*m_H -5);
	glColor3f(m_Color.x, 1.0, m_Color.z);
	glVertex2f( 0.4*m_W, 0.05*m_H -5);
	glVertex2f( 0.4*m_W, 0.05*m_H +5);

	glColor3f(m_Color.x, m_Color.y, 0.0);
	glVertex2f(-0.4*m_W, -0.1*m_H +5);
	glVertex2f(-0.4*m_W, -0.1*m_H -5);
	glColor3f(m_Color.x, m_Color.y, 1.0);
	glVertex2f( 0.4*m_W, -0.1*m_H -5);
	glVertex2f( 0.4*m_W, -0.1*m_H +5);

	glEnd();

	//Lines on the color slides
	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
	glVertex2f(0.8*m_W*(m_Color.x-0.5), 0.2*m_H -7);
	glVertex2f(0.8*m_W*(m_Color.x-0.5), 0.2*m_H +7);

	glVertex2f(0.8*m_W*(m_Color.y-0.5), 0.05*m_H -7);
	glVertex2f(0.8*m_W*(m_Color.y-0.5), 0.05*m_H +7);

	glVertex2f(0.8*m_W*(m_Color.z-0.5), -0.1*m_H -7);
	glVertex2f(0.8*m_W*(m_Color.z-0.5), -0.1*m_H +7);
	glEnd();

	//Title
	glPushMatrix();
	glTranslatef(-0.5*theConsoleFont->getFontW()*m_Title.size(), 0.3*m_H, 0);

	theConsoleFont->enable();
	theConsoleFont->drawString(m_Title);
	theConsoleFont->disable();
	glPopMatrix();

	//OK button
	glPushMatrix();
	glTranslatef(-0.4*m_W, -0.3*m_H, 0);

	if(m_Selected == eOK)
		{glColor3f(1, 0.9, 0);}
	else
		{glColor3f(1, 1, 1);}
	theConsoleFont->enable();
	theConsoleFont->drawString(_("OK"));
	theConsoleFont->disable();
	glPopMatrix();

	//Cancel button
	glPushMatrix();
	glTranslatef(0.2*m_W, -0.3*m_H, 0);

	if(m_Selected == eCancel)
		{glColor3f(1, 0.9, 0);}
	else
		{glColor3f(1, 1, 1);}
	theConsoleFont->enable();
	theConsoleFont->drawString(_("Cancel"));
	theConsoleFont->disable();
	glPopMatrix();

	return 0;
}

