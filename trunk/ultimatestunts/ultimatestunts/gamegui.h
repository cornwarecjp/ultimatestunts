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
#include "guipage.h"

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
	virtual ~CGameGUI();

	virtual void start(); //returns when an exit command is given


protected:
	//Core things that are to be managed
	CUSCore *m_GameCore;
	vector<CPlayer *> m_Players;
	CUSServer *m_Server;

	//all data of the last times the menus were passed
	enum eGameType
	{
		LocalGame=1,
		NewNetwork,
		JoinNetwork,
	} m_GameType;
	CString m_HostName;
	int m_HostPort;
	CString m_TrackFile;
	int m_MaxNumPlayers;

	struct SPlayerDescr
	{
		CString name;
		bool isHuman;
		unsigned int carIndex; //refers to m_CarFile
	};
	vector<SPlayerDescr> m_PlayerDescr;
	unsigned int m_SelectedPlayer; //for player menus

	//A cache for names of the cars, loaded by constructor
	struct SCarFile
	{
		CString filename;
		CString fullname;
	};
	vector<SCarFile> m_CarFiles;

	void updateMenuTexts();

	//The menu starters:
	CString viewMainMenu();
		CString playGame();
			void load();
			void unload();
		CString viewGameTypeMenu();
		CString viewTrackMenu();
		CString viewPlayersMenu();
		CString viewPlayerMenu();
		CString viewCarMenu();
		CString viewHiscore();

	//The menu pages:
	CGUIPage m_MainPage;
	CGUIPage m_GameTypePage;
	CGUIPage m_TrackPage;
	CGUIPage m_PlayersPage;
	CGUIPage m_PlayerPage;
	CGUIPage m_CarPage;
	CGUIPage m_LoadingPage;
	CGUIPage m_HiscorePage;
};

#endif
