/***************************************************************************
                          gui.cpp  -  The graphical user interface: menu's etc.
                             -------------------
    begin                : vr jan 31 2003
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

#include <cstdio>

#include "gui.h"

CGUI::CGUI(const CLConfig &conf, CWinSystem *winsys)
{
	//TODO: get conffile options
	m_WinSys = winsys;
}

CGUI::~CGUI(){
}

CString CGUI::getInput()
{
	char input[80];
	fgets(input, 79, stdin);
	CString ret(input);
	ret = ret.mid(0, ret.length() - 1); //remove \n
	return ret;
}
