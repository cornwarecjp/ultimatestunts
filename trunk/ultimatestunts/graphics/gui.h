/***************************************************************************
                          gui.h  -  The graphical user interface: menu's etc.
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

#ifndef GUI_H
#define GUI_H

#include "lconfig.h"
#include "winsystem.h"

/**
  *@author CJP
  */

class CGUI //derived from CBThread in the future
{
public:

	enum eMenu {
		MainMenu=1,
		HostMenu,
		TrackMenu,
		PlayerMenu,
		Options,
		CarMenu,
		HighScore,
		NoMenu
	};

	CGUI(const CLConfig &conf, CWinSystem *winsys);
	~CGUI();

	void startFrom(eMenu menu);
	void stop();

	const void *getData(eMenu menu, CString item);

protected:
	CWinSystem *m_WinSys;

	bool m_Stop;

	bool m_PassedMainMenu, m_PassedHostMenu;

	//all data of the last times the menus were passed
	int m_MainMenuInput;
	CString m_HostName;
	int m_HostPort;

	//The menu code
	eMenu viewMainMenu();
	eMenu viewHostMenu();
};

#endif
