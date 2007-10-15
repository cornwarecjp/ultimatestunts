/***************************************************************************
                          replaceactionwidget.h  -  Widget interface for CReplaceAction
                             -------------------
    begin                : zo jul 15 2007
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
#ifndef REPLACEACTIONWIDGET_H
#define REPLACEACTIONWIDGET_H

#include "actionwidget.h"

#include "tileselect.h"
#include "iconbar.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CReplaceActionWidget : public CActionWidget {
public:
	CReplaceActionWidget();
	virtual ~CReplaceActionWidget();

	virtual int onKeyPress(int key);

	virtual int onMouseClick(int x, int y, unsigned int buttons);
	virtual int onMouseMove(int x, int y, unsigned int buttons);
	virtual int onResize(int x, int y, int w, int h);
	virtual int onIdle();
	virtual int onRedraw();

protected:
	CTileSelect m_TileSelect;
	CIconBar m_IconBar;

	void rotate();
	void insert();
	void apply();
	void del();
};

#endif
