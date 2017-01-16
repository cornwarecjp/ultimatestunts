/***************************************************************************
                          label.cpp  -  A simple text label widget
                             -------------------
    begin                : za jun 9 2007
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
#include <GL/gl.h>

#include "console.h"

#include "label.h"

CLabel::CLabel()
 : CWidget()
{
	loadConsoleFont();
}


CLabel::~CLabel()
{
}

int CLabel::onRedraw()
{
	CWidget::onRedraw();

	glPushMatrix();
	glTranslatef(m_W/2 - 0.5*theConsoleFont->getFontW()*m_Text.size(), 0.0, 0.0);

	theConsoleFont->enable();
	theConsoleFont->drawString(m_Text);
	theConsoleFont->disable();
	glPopMatrix();

	return 0;
}

