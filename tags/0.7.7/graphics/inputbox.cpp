/***************************************************************************
                          inputbox.cpp  -  Getting user input
                             -------------------
    begin                : di jan 11 2005
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

#include "SDL.h"
#include <GL/gl.h>

#include "inputbox.h"
#include "console.h"

CInputBox::CInputBox()
{
	loadConsoleFont();

	m_Text = "Text";
	m_Cancelled = false;
}

CInputBox::~CInputBox(){
}

int CInputBox::onKeyPress(int key)
{
	if(key == SDLK_ESCAPE) m_Cancelled = true;

	if(key == SDLK_RETURN || key == SDLK_ESCAPE) return WIDGET_QUIT;

	if(key > 255 || key < 0) return 0; //ignore key

	if(key == SDLK_BACKSPACE)
	{
		if(m_Text.length() > 0)
			m_Text = m_Text.mid(0, m_Text.length()-1);

		return WIDGET_REDRAW;
	}

	m_Text += (char)key;

	return WIDGET_REDRAW;
}

int CInputBox::onMouseClick(int x, int y, unsigned int buttons)
{
	return WIDGET_QUIT;
}

int CInputBox::onRedraw()
{
	CWidget::onRedraw();

	glColor4f(0, 0, 0, 0.9);
	drawBackground();
	glColor4f(1,1,1,1);

	//centered
	glTranslatef(m_W/2, m_H/2, 0);

	glPushMatrix();

	glTranslatef(-0.5*theConsoleFont->getFontW()*m_Text.size(), -0.3*m_H, 0);

	//the text
	glColor3f(1, 0.9, 0);
	theConsoleFont->enable();
	theConsoleFont->drawString(m_Text);
	theConsoleFont->disable();

	//the cursor:
	glBindTexture(GL_TEXTURE_2D, 0); //no texture
	glBegin(GL_LINES);
	glVertex2f(2,0);
	glVertex2f(2,theConsoleFont->getFontH());
	glEnd();

	glPopMatrix();

	//the title
	glColor3f(1,1,1);
	drawTitle();

	return 0;
}
