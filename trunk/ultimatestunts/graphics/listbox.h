/***************************************************************************
                          listbox.h  -  A List Box
                             -------------------
    begin                : za mrt 8 2008
    copyright            : (C) 2008 by CJP
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
#ifndef LISTBOX_H
#define LISTBOX_H

#include <vector>
namespace std {}
using namespace std;

#include "widget.h"
#include "longmenu.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CListBox : public CWidget
{
public:
	CListBox();
	~CListBox();

	void setTitle(const CString &title);

	virtual int onKeyPress(int key);
	virtual int onMouseClick(int x, int y, unsigned int buttons);
	virtual int onMouseMove(int x, int y, unsigned int buttons);
	virtual int onRedraw();
	virtual int onResize(int x, int y, int w, int h);

	void setOptions(const vector<CString> &options);

	void setSelected(unsigned int sel);
	unsigned int getSelected() const;

	bool m_Cancelled;
protected:
	CString m_Title; //the total text
	vector<CString> m_Lines; //the text divided into lines

	CLongMenu m_Menu;

	void updateLines();

	void drawTitle();
};

#endif
