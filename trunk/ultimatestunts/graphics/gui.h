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

#include <vector>
namespace std {}
using namespace std;

#include "lconfig.h"
#include "winsystem.h"

/**
  *@author CJP
  */

class CGUI //derived from CBThread in the future
{
public:

	enum eMainMenu {
		LocalGame=1,
		NewNetwork,
		JoinNetwork,
		Options,
		LoadReplay,
		EditTrack,
		Exit
	};

	CGUI(const CLConfig &conf, CWinSystem *winsys);
	~CGUI();

	void startFrom(CString section);
	void stop();

	//new interface:
	CString getValue(CString section, CString field="");
	void resetSection(CString section="");
	bool isPassed(CString section);

protected:
	CWinSystem *m_WinSys;

	bool m_Stop;

	bool	m_PassedMainMenu,
				m_PassedHostMenu,
				m_PassedServerMenu,
				m_PassedTrackMenu,
				m_PassedPlayerMenu;

	//all data of the last times the menus were passed
	eMainMenu m_MainMenuInput;
	CString m_HostName;
	int m_HostPort;
	int m_ServerPort;
	CString m_TrackFile;

	struct SPlayerDescr
	{
		CString name;
		bool isHuman;
		CString carFile;
	};
	vector<SPlayerDescr> m_PlayerDescr;
	

	//The menu code
	CString viewMainMenu();
	CString viewHostMenu();
	CString viewServerMenu();
	CString viewTrackMenu();
	CString viewPlayerMenu();

	void addPlayer(CString name, bool human, CString carfile);

	CString getInput();
};

#endif
