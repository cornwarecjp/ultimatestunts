/***************************************************************************
                          console.h  -  Basic text I/O (using e.g. openGL)
                             -------------------
    begin                : ma okt 25 2004
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

#ifndef CONSOLE_H
#define CONSOLE_H

#include <vector>
namespace std {}
using namespace std;

#include "cstring.h"
#include "winsystem.h"
#include "font.h"

/**
  *@author CJP
  */

class CConsole {
public: 
	CConsole(CWinSystem *winsys);
	virtual ~CConsole();

	virtual void print(const CString &str);
	virtual CString getInput(const CString &question = "");
	virtual void clear();
	virtual void clearScreen();
	virtual void draw();

	virtual void enterWriteMode();
	virtual void leaveWriteMode();

protected:
	CWinSystem *m_WinSystem;
	CFont m_Font;

	bool m_WriteMode;

	vector<CString> m_ScreenContent;
	float m_fontW, m_fontH;
};

#endif
