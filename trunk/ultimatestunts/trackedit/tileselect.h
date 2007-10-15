/***************************************************************************
                          tileselect.h  -  A tile selection widget
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
#ifndef TILESELECT_H
#define TILESELECT_H

#include "renderwidget.h"
#include "objectviewer.h"
#include "tecamera.h"

#include "actionwidget.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CTileSelect : public CWidget
{
public:
	CTileSelect();
	virtual ~CTileSelect();

	virtual int onMouseClick(int x, int y, unsigned int buttons);
	virtual int onMouseMove(int x, int y, unsigned int buttons);

	virtual int onIdle();
	virtual int onRedraw();

	void setSelection(unsigned int sel);
	unsigned int getSelection() const
		{return m_Selection;}
protected:
	CRenderWidget *m_RenderWidget;
	CObjectViewer *m_ObjectViewer;
	CTECamera m_Camera;

	unsigned int m_Selection;
};

#endif
