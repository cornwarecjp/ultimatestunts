/***************************************************************************
                          guipage.h  -  A page of a menu interface
                             -------------------
    begin                : wo dec 15 2004
    copyright            : (C) 2004 by CJP
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

#ifndef GUIPAGE_H
#define GUIPAGE_H

#include <vector>
namespace std {}
using namespace std;

#include "widget.h"
#include "cstring.h"

/**
  *@author CJP
  */

class CGUIPage : public CWidget  {
public: 
	CGUIPage();
	~CGUIPage();

	virtual int onKeyPress(int key);
	virtual int onMouseMove(int x, int y);
	virtual int onMouseClick(int x, int y, unsigned int buttons);
	virtual int onResize(int x, int y, int w, int h);
	virtual int onRedraw();

	vector<CWidget *> m_Widgets;
	CString m_Title;
};

#endif
