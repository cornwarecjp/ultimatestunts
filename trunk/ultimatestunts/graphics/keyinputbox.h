/***************************************************************************
                          keyinputbox.h  -  Asking the user to enter a key
                             -------------------
    begin                : do dec 23 2010
    copyright            : (C) 2010 by CJP
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
#ifndef KEYINPUTBOX_H
#define KEYINPUTBOX_H

#include "messagebox.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CKeyInputBox : public CMessageBox
{
public:
	CKeyInputBox();
	virtual ~CKeyInputBox();
	
	virtual int onKeyPress(int key);
	virtual int onMouseClick(int x, int y, unsigned int buttons);
	virtual int onRedraw();

	int m_Key;
	bool m_Cancelled;
};

#endif
