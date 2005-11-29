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

#include <vector>
namespace std {}
using namespace std;

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
	virtual int onResize(int x, int y, int w, int h);
	virtual int onMouseMove(int x, int y);
	virtual int onMouseClick(int x, int y, unsigned int buttons);

	void setTitle(const CString &title);

	enum {eYesNo, eOK} m_Type;
	unsigned int m_Selected;

protected:
	CString m_Title; //the total text
	vector<CString> m_Lines; //the text divided into lines

	void updateLines();
};

#endif
