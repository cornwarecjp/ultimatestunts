/***************************************************************************
                          messagebox.h  -  A Message Box
                             -------------------
    begin                : wo jan 12 2005
    copyright            : (C) 2005 by CJP
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

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include "widget.h"
#include "cstring.h"

/**
  *@author CJP
  */

class CMessageBox : public CWidget  {
public: 
	CMessageBox();
	virtual ~CMessageBox();

	virtual int onKeyPress(int key);
	virtual int onRedraw();
	virtual int onMouseMove(int x, int y);
	virtual int onMouseClick(int x, int y, unsigned int buttons);

	enum {eYesNo, eOK} m_Type;
	CString m_Title;
	unsigned int m_Selected;
};

#endif
