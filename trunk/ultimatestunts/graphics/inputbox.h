/***************************************************************************
                          inputbox.h  -  Getting user input
                             -------------------
    begin                : di jan 11 2005
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

#ifndef INPUTBOX_H
#define INPUTBOX_H

#include "widget.h"
#include "cstring.h"

/**
  *@author CJP
  */

class CInputBox : public CWidget  {
public: 
	CInputBox();
	virtual ~CInputBox();

	virtual int onKeyPress(int key);
	virtual int onRedraw();
	virtual int onMouseClick(int x, int y, unsigned int buttons);

	CString m_Title;
	CString m_Text;
};

#endif
