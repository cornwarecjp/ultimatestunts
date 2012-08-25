/***************************************************************************
                          fileselect.h  -  A File Selection tool
                             -------------------
    begin                : zo feb 04 2007
    copyright            : (C) 2007 by CJP
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

#ifndef FILESELECT_H
#define FILESELECT_H

#include <vector>
namespace std {}
using namespace std;

#include "widget.h"
#include "cstring.h"
#include "longmenu.h"

/**
  *@author CJP
  */

class CFileSelect : public CWidget  {
public: 
	CFileSelect();
	virtual ~CFileSelect();

	virtual int onKeyPress(int key);
	virtual int onRedraw();
	virtual int onResize(int x, int y, int w, int h);
	virtual int onMouseMove(int x, int y, unsigned int buttons);
	virtual int onMouseClick(int x, int y, unsigned int buttons);

	void setTitle(const CString &title);

	void setExtension(const CString &ext)
		{m_Extension = ext; updateDirListing();}
	CString getFullName()
		{return m_Directory + m_File;}

	bool m_Cancelled;

protected:
	CString m_Title; //the total text
	CLongMenu m_Menu;

	static CString m_Directory; //Value stays in memory for next file selector
	CString m_File;
	CString m_Extension;

	void updateDirListing();
	int selectEntry();
	void enterDirectory(const CString &dirname);

	enum {eCancel, eNone} m_Selected;
};

#endif
