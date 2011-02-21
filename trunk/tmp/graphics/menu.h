/***************************************************************************
                          menu.h  -  A menu GUI element
                             -------------------
    begin                : do dec 16 2004
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

#ifndef MENU_H
#define MENU_H

#include <vector>
namespace std {}
using namespace std;

#include "widget.h"
#include "cstring.h"

/**
  *@author CJP
  */

class CMenu : public CWidget  {
public: 
	CMenu();
	virtual ~CMenu();

	virtual int onKeyPress(int key);
	virtual int onMouseMove(int x, int y, unsigned int buttons);
	virtual int onMouseClick(int x, int y, unsigned int buttons);
	virtual int onRedraw();

	unsigned int m_Selected;
	vector<CString> m_Lines;
	bool m_AlignLeft;

protected:
	virtual int getdy();
	
};

#endif
