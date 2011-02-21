/***************************************************************************
                          objectviewwidget.cpp  -  CRenderWidget for a CObjectViewer
                             -------------------
    begin                : za okt 20 2007
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

#include <cmath>
#include "pi.h"

#include "usmacros.h"
#include "timer.h"

#include "objectviewwidget.h"

#define HORDST (3*TILESIZE)
#define VERDST (TILESIZE)

CObjectViewWidget::CObjectViewWidget(CDataManager *world)
 : m_ObjectViewer(world)
{
	m_Renderer = &m_ObjectViewer;
	m_ObjectViewer.setCamera(&m_Camera);

	CVector pos(0.0,VERDST,-HORDST);
	CMatrix ori;
	ori.targetZ(pos, true);

	m_Camera.setPosition(pos);
	m_Camera.setOrientation(ori);
}


CObjectViewWidget::~CObjectViewWidget()
{
}

int CObjectViewWidget::onIdle()
{
	CTimer t;
	float time = t.getTime();

	CVector pos(HORDST*sin(0.5*time),VERDST,HORDST*cos(0.5*time));
	CMatrix ori;
	ori.targetZ(pos, true);

	m_Camera.setPosition(pos);
	m_Camera.setOrientation(ori);

	return WIDGET_REDRAW;
}

