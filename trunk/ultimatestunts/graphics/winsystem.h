/***************************************************************************
                          winsystem.h  -  description
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

/**
  *@author CJP
  */

class CWinSystem {
public: 
	CWinSystem();
	~CWinSystem();

	int runLoop( bool (CALLBACKFUN *loopfunc)() );

protected:
	void reshape(int w, int h);
};

#endif