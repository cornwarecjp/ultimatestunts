/***************************************************************************
                          keyinputbox.cpp  -  Asking the user to enter a key
                             -------------------
    begin                : do dec 23 2010
    copyright            : (C) 2010 by CJP
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

#include <libintl.h>
#define _(String) gettext (String)

#include "keyinputbox.h"
#include "console.h"

CKeyInputBox::CKeyInputBox()
	: CMessageBox()
{
	m_Cancelled = false;
}


CKeyInputBox::~CKeyInputBox()
{
}


int CKeyInputBox::onKeyPress(int key)
{
	m_Key = key;
	return WIDGET_QUIT;
}

int CKeyInputBox::onMouseClick(int x, int y, unsigned int buttons)
{
	return 0;
}

int CKeyInputBox::onRedraw()
{
	CWidget::onRedraw();

	glColor4f(0, 0, 0, 0.9);
	drawBackground();
	glColor4f(1,1,1,1);

	//centered
	glTranslatef(m_W/2, m_H/2, 0);

	glPushMatrix();

	CString text = _("<press a key>");

	glTranslatef(-0.5*theConsoleFont->getFontW()*text.size(), -0.3*m_H, 0);

	//the text
	glColor3f(1, 0.9, 0);
	theConsoleFont->enable();
	theConsoleFont->drawString(text);
	theConsoleFont->disable();

	glPopMatrix();

	//the title
	glColor3f(1,1,1);
	drawTitle();

	return 0;
}

