/***************************************************************************
                          winsystem.h  -  A window managing class
                             -------------------
    begin                : wo jan 15 2003
    copyright            : (C) 2003 by CJP
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

#ifndef WINSYSTEM_H
#define WINSYSTEM_H

#include "usmacros.h"
#include "lconfig.h"

/**
  *@author CJP
  */

class CWinSystem {
public: 
	CWinSystem(const CLConfig &conf);
	~CWinSystem();

	int runLoop( bool (CALLBACKFUN *loopfunc)(), bool swp = false);

protected:
	int m_W, m_H, m_BPP;
	Uint32 m_Flags;
	void reshape(int w, int h);

	int m_VisibleTiles;
};

#endif
