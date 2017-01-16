/***************************************************************************
                          objectviewwidget.h  -  CRenderWidget for a CObjectViewer
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
#ifndef OBJECTVIEWWIDGET_H
#define OBJECTVIEWWIDGET_H

#include "objectviewer.h"
#include "camera.h"

#include "renderwidget.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CObjectViewWidget : public CRenderWidget
{
public:
	CObjectViewWidget(CDataManager *world);
	~CObjectViewWidget();

	virtual int onIdle();

	CObjectViewer m_ObjectViewer;
	CCamera m_Camera;
};

#endif
