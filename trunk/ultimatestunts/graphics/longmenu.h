/***************************************************************************
                          longmenu.h  -  menu with scrolling ability
                             -------------------
    begin                : ma jan 2 2006
    copyright            : (C) 2006 by CJP
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

#ifndef LONGMENU_H
#define LONGMENU_H

#include "menu.h"

/**
  *@author CJP
  */

class CLongMenu : public CMenu  {
public: 
	CLongMenu();
	virtual ~CLongMenu();

	virtual int onKeyPress(int key);
	virtual int onMouseMove(int x, int y, unsigned int buttons);
	virtual int onMouseClick(int x, int y, unsigned int buttons);
	virtual int onRedraw();

protected:
	virtual int getdy();
	unsigned int getNumVisibleLines();
	unsigned int getScreenTop();
};

#endif
