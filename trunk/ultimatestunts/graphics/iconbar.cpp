/***************************************************************************
                          iconbar.cpp  -  A toolbar with icons
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
#include <cstdio>
#include <GL/gl.h>

#include "iconbar.h"

CIconBar::CIconBar()
 : CWidget()
{
	m_Selected = CIconList::eNone;

	//Load the icon list if necessary
	if(theIconList == NULL)
		theIconList = new CIconList;

}


CIconBar::~CIconBar()
{
}

int CIconBar::onRedraw()
{
	CWidget::onRedraw();

	glPushMatrix();
	glTranslatef(16, m_H/2, 0);

	theIconList->enable();

	for(unsigned int i=0; i < m_Icons.size(); i++)
	{
		glTranslatef(32 * m_Icons[i].xpos, 0, 0);
		theIconList->draw(m_Icons[i].ID);

		if(m_Icons[i].ID == m_Selected)
		{
			theIconList->disable();
			glBindTexture(GL_TEXTURE_2D, 0); //no texture
			glBegin(GL_LINE_LOOP);
			glVertex2f(-16,-16);
			glVertex2f(-16, 16);
			glVertex2f( 16, 16);
			glVertex2f( 16,-16);
			glEnd();
			theIconList->enable();
		}
	}

	theIconList->disable();

	glPopMatrix();

	return 0;
}

int CIconBar::onMouseMove(int x, int y, unsigned int buttons)
{
	float midPos = m_X+16;
	CIconList::eIconID sel = CIconList::eNone;
	int sel_i = -1;

	for(unsigned int i=0; i < m_Icons.size(); i++)
	{
		midPos += 32*m_Icons[i].xpos;

		if(x > midPos-16 && x < midPos+16)
		{
			sel = m_Icons[i].ID;
			sel_i = i;
		}
	}

	if(sel != m_Selected)
	{
		m_Selected = sel;

		if(sel_i >= 0)
			{m_Description = m_Icons[sel_i].description;}
		else
			{m_Description = "";}

		//printf("%s\n", m_Description.c_str());

		return WIDGET_REDRAW;
	}

	return 0;
}

int CIconBar::onMouseClick(int x, int y, unsigned int buttons)
{
	int ret = onMouseMove(x, y, buttons);

	if(m_Selected == CIconList::eNone) return ret;

	return ret | WIDGET_QUIT;
}
