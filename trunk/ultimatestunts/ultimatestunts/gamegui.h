/***************************************************************************
                          gamegui.h  -  The Game menu interface
                             -------------------
    begin                : do okt 14 2004
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

#ifndef GAMEGUI_H
#define GAMEGUI_H

#include "gamewinsystem.h"
#include "gui.h"
#include "console.h"

#include "uscore.h"
#include "player.h"
#include "lconfig.h"

#include "usserver.h"


/**
  *@author CJP
  */

class CGameGUI : public CGUI  {
public: 
	CGameGUI(const CLConfig &conf, CGameWinSystem *winsys);
	~CGameGUI();

	void start(); //returns when an exit command is given


protected:
	//Core things that are to be managed
	CUSCore *m_GameCore;
	vector<CPlayer *> m_Players;
	CUSServer *m_Server;

	CConsole *m_Console;

	//all data of the last times the menus were passed
	enum eMainMenu
	{
		LocalGame=1,
		NewNetwork,
		JoinNetwork,
		Exit
	} m_MainMenuInput;
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


	//The menu's
	CString viewMainMenu();
	CString viewHostMenu();
	CString viewServerMenu();
	CString viewTrackMenu();

	CString viewPlayerMenu();
	void addPlayer(CString name, bool human, CString carfile);

	CString playGame();
	void load();
	void unload();

	CString viewHiscore();
};

#endif
