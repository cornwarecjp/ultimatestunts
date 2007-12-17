/***************************************************************************
                          tileselect.cpp  -  A tile selection widget
                             -------------------
    begin                : zo jun 10 2007
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
#include "SDL.h"

#include "trackdocument.h"
#include "winsystem.h"

#include "replaceaction.h"

#include "tileselect.h"

CTileSelect::CTileSelect()
{
	m_RenderWidget = new CObjectViewWidget(theTrackDocument->m_DataManager);
	m_Selection = 1;
}


CTileSelect::~CTileSelect()
{
	delete m_RenderWidget;
}

void CTileSelect::setSelection(unsigned int sel)
{
	m_Selection = sel;
	if(m_Selection == 0) //0 = delete
		m_Selection = 
			theTrackDocument->m_DataManager->getNumObjects(CDataObject::eTileModel) - 1;

	if(m_Selection >= 
		theTrackDocument->m_DataManager->getNumObjects(CDataObject::eTileModel))
			m_Selection = 1; //0 = delete
}

int CTileSelect::onMouseClick(int x, int y, unsigned int buttons)
{
	if(isInScrollbar(x, y))
		return 0;

	x -= m_X;
	y -= m_Y;
	y = m_H - y;
	y += m_ScrollPosition;

	int cols = m_W / 64;
	int index = 1 + cols * (y/64) + (x/64);

	const CTEManager *manager = theTrackDocument->m_DataManager;
	int numTiles = manager->getNumObjects(CDataObject::eTileModel);

	if(index < 0) index = 0;
	if(index >= numTiles) index = numTiles-1;

	m_Selection = index;

	return WIDGET_REDRAW | WIDGET_QUIT;
}

int CTileSelect::onMouseMove(int x, int y, unsigned int buttons)
{
	//Scrollbar handling
	if(isInScrollbar(x, y))
	{
		if(handleScrollbarMove(x, y, buttons))
			return WIDGET_REDRAW;

		return 0;
	}

	return 0;
}

int CTileSelect::onIdle()
{
	return m_RenderWidget->onIdle();
}

int CTileSelect::onRedraw()
{
	CWidget::onRedraw();

	const CTEManager *manager = theTrackDocument->m_DataManager;
	vector<const CDataObject *> tiles = manager->getObjectArray(CDataObject::eTileModel);

	if(tiles.size() < 2)
	{
		m_RequestH = -1;
		return 0;
	}

	int cols = m_W / 64;
	m_RequestH = 64 * (1 + (tiles.size() / cols));

	if(m_RequestH < m_H)
		{m_ScrollPosition = 0;}
	else if(m_ScrollPosition > m_RequestH - m_H)
		{m_ScrollPosition = m_RequestH - m_H;}

	int ret = 0;

	if(m_Selection >= tiles.size())
	{
		m_Selection = tiles.size() - 1;
	}

	for(unsigned int i=1; i < tiles.size(); i++)
	{
		int x = (i-1) % cols;
		int y = (i-1) / cols;

		x = 64*x;
		y = 64*y;
		y -= m_ScrollPosition;

		if(y < -64) continue;
		if(y > m_H) break;

		int h = 64;

		if(y < 0)
		{
			h = 64 + y;
			y = 0;
		}

		if(y+h > m_H) h = m_H - y;

		m_RenderWidget->onResize(m_X + x, m_Y+m_H - y - h, 64, h);

		const CDataObject *tile = (const CDataObject *)(tiles[i]);
		m_RenderWidget->m_ObjectViewer.m_Objects.clear();
		m_RenderWidget->m_ObjectViewer.addObject(tile->getFilename(), tile->getParamList(),
			CVector(0,0,0), CMatrix(), false, CDataObject::eTileModel);

		glPushMatrix();
		ret |= m_RenderWidget->onRedraw();
		glPopMatrix();

		if(i == m_Selection)
		{
			//Selection rectangle
			glColor3f(1.0,0.8,0.0);
			glBindTexture(GL_TEXTURE_2D, 0); //no texture
			glBegin(GL_LINE_LOOP);
			glVertex2f(x       ,m_H - y - h + 2);
			glVertex2f(x       ,m_H - y);
			glVertex2f(x+64 - 2,m_H - y);
			glVertex2f(x+64 - 2,m_H - y - h + 2);
			glEnd();
			glColor3f(1,1,1);
		}
	}

	glScissor(m_X, m_Y, m_W, m_H); //reset scissor to current widget

	drawScrollbar();

	return ret;
}

