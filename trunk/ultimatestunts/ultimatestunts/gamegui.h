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
#include "carviewer.h"

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
		ViewReplay
	} m_GameType;
	void initGameType(bool keepServerAlive=false, bool keepReplayFile=false);

	CString m_HostName;
	int m_HostPort;
	int m_MinPlayers;
	CString m_ServerName;
	vector<CClientNet::SBroadcastResult> m_ServerList;
	CString m_TrackFile;
	CString m_ReplayFile;

	struct SPlayerDescr
	{
		CString name;
		bool isHuman;
		unsigned int carIndex; //refers to m_CarFile
		CVector carColor;
	};
	vector<SPlayerDescr> m_PlayerDescr;
	unsigned int m_SelectedPlayer; //for player menus

	//A cache for names of the cars, loaded by constructor
	struct SCarFile
	{
		CString filename;
		CString fullname;
		CVector defaultColor;
	};
	vector<SCarFile> m_CarFiles;

	void updateMenuTexts();

	//The menu starters:
	CString viewMainMenu();
		CString playGame();
			void load();
			void unload();
		CString viewGameTypeMenu();
		CString viewSelectServerMenu();
		CString viewTrackMenu();
		CString viewReplayMenu();
		CString viewReplay();
		CString viewPlayersMenu();
		CString viewPlayerMenu();
		CString viewCarMenu();
		CString viewCreditsMenu();
		CString viewHiscore();

	//The menu pages:
	CGUIPage m_MainPage;
	CGUIPage m_GameTypePage;
	CGUIPage m_SelectServerPage;
	CGUIPage m_TrackPage;
	CGUIPage m_ReplayPage;
	CGUIPage m_PlayersPage;
	CGUIPage m_PlayerPage;
	CGUIPage m_CarPage;
	CGUIPage m_CreditsPage;
	CGUIPage m_LoadingPage;
	CGUIPage m_HiscorePage;

	CCarViewer *m_CarViewer;
};

#endif
