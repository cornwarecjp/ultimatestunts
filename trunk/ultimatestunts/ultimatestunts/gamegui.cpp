/***************************************************************************
                          gamegui.cpp  -  The Game menu interface
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

#include <cstdio>

#include "gamegui.h"
#include "guipage.h"

#include "objectchoice.h"
#include "aiplayercar.h"
#include "humanplayer.h"

#include "datafile.h"

CUSCore *_USGameCore = NULL;

bool game_mainloop()
{
	return _USGameCore->update();
}


CGameGUI::CGameGUI(const CLConfig &conf, CGameWinSystem *winsys) : CGUI(conf, winsys)
{
	m_GameCore = new CUSCore(winsys);

	m_Server = NULL;

	m_ChildWidget = new CGUIPage;

	//default values:
	m_GameType = LocalGame;
	m_TrackFile = "tracks/default.track";
	addPlayer("CJP", true, "cars/diablo.conf");
	addPlayer("AI", false, "cars/f1.conf");
}

CGameGUI::~CGameGUI()
{
	delete m_GameCore;

	if(m_Server != NULL)
		{delete m_Server; m_Server = NULL;}

	delete m_ChildWidget;
}

void CGameGUI::start()
{
	enter2DMode();

	CString section = "mainmenu";
	while(section != "" && section != "exit")
	{
		if(section=="mainmenu")
			section = viewMainMenu();
		else if(section=="gametypemenu")
			section = viewGameTypeMenu();
		else if(section=="trackmenu")
			section = viewTrackMenu();
		else if(section=="playermenu")
			section = viewPlayerMenu();
		else if(section=="playgame")
			section = playGame();
		else if(section=="hiscore")
			section = viewHiscore();
		else
			{printf("Error: unknown menu\n");}
	}

	leave2DMode();
}

CString CGameGUI::viewMainMenu()
{
	CGUIPage *page = (CGUIPage *)m_ChildWidget;
	page->m_Menu.m_Selected = 0;
	page->m_Title = "Main menu";
	page->m_Menu.m_Lines.clear();
	page->m_Menu.m_Lines.push_back("Start playing");
	page->m_Menu.m_Lines.push_back("Set the game type");
	page->m_Menu.m_Lines.push_back("Select the track");
	page->m_Menu.m_Lines.push_back("Select the players");
	page->m_Menu.m_Lines.push_back("Options");
	page->m_Menu.m_Lines.push_back("Exit");
	m_WinSys->runLoop(this);

	switch(page->m_Menu.m_Selected)
	{
		case 0:
			return "playgame";
		case 1:
			return "gametypemenu";
		case 2:
			return "trackmenu";
		case 3:
			return "playermenu";
		case 4:
			page->m_Title = "Options";
			page->m_Menu.m_Lines.clear();
			page->m_Menu.m_Lines.push_back("Please edit ultimatestunts.conf manually");
			page->m_Menu.m_Selected = 0;
			m_WinSys->runLoop(this);
			return "mainmenu";
		case 5:
			return "exit";
	}

	return "";
}

CString CGameGUI::viewGameTypeMenu()
{
	CGUIPage *page = (CGUIPage *)m_ChildWidget;
	page->m_Menu.m_Selected = 0;
	page->m_Title = "Select the game type:";
	page->m_Menu.m_Lines.clear();
	page->m_Menu.m_Lines.push_back("Local game");
	page->m_Menu.m_Lines.push_back("Join an existing network game");
	page->m_Menu.m_Lines.push_back("Start a new network game");
	m_WinSys->runLoop(this);

	switch(page->m_Menu.m_Selected)
	{
		case 0:
			m_GameType = LocalGame;
			return "mainmenu";
		case 1:
			m_GameType = JoinNetwork;
			//TODO:
			//m_HostName = m_Console->getInput("Enter the host name: ");
			//m_HostPort = m_Console->getInput("Enter the port number: ").toInt();
			return "mainmenu";
		case 2:
			m_GameType = NewNetwork;
			//TODO:
			//m_ServerPort = m_Console->getInput("Enter the port number: ").toInt();
			return "mainmenu";
	}

	return "mainmenu";
}

CString CGameGUI::viewTrackMenu()
{
	CGUIPage *page = (CGUIPage *)m_ChildWidget;
	page->m_Menu.m_Selected = 0;
	page->m_Title = "Select a track:";
	page->m_Menu.m_Lines = getDirContents("tracks", ".track");
	m_WinSys->runLoop(this);

	m_TrackFile = CString("tracks/") + page->m_Menu.m_Lines[page->m_Menu.m_Selected];
	return "mainmenu";
}

CString CGameGUI::viewPlayerMenu()
{
	CGUIPage *page = (CGUIPage *)m_ChildWidget;

	m_PlayerDescr.clear();

	CString name = "Player 1";

	page->m_Menu.m_Selected = 0;
	page->m_Title = "Select a car for yourself:";
	page->m_Menu.m_Lines = getDirContents("cars", ".conf");
	m_WinSys->runLoop(this);
	CString carfile = CString("cars/") + page->m_Menu.m_Lines[page->m_Menu.m_Selected];

	addPlayer(name, true, carfile);

	while(true)
	{
		page->m_Menu.m_Selected = 0;
		page->m_Title = "Do you want to add a player?";
		page->m_Menu.m_Lines.clear();
		page->m_Menu.m_Lines.push_back("yes");
		page->m_Menu.m_Lines.push_back("no");
		m_WinSys->runLoop(this);
		if(page->m_Menu.m_Selected == 1) break;

		name = "Player";

		page->m_Menu.m_Selected = 0;
		page->m_Title = "Select a car for this player:";
		page->m_Menu.m_Lines = getDirContents("cars", ".conf");
		m_WinSys->runLoop(this);
		CString carfile = CString("cars/") + page->m_Menu.m_Lines[page->m_Menu.m_Selected];

		page->m_Menu.m_Selected = 0;
		page->m_Title = "Is it an AI player?";
		page->m_Menu.m_Lines.clear();
		page->m_Menu.m_Lines.push_back("yes");
		page->m_Menu.m_Lines.push_back("no");
		m_WinSys->runLoop(this);
		bool isHuman = (page->m_Menu.m_Selected == 1);
		addPlayer(name, isHuman, carfile);
	}

	return "mainmenu";
}

void CGameGUI::addPlayer(CString name, bool human, CString carfile)
{
	SPlayerDescr pd;
	pd.name = name;
	pd.isHuman = human;
	pd.carFile = carfile;
	m_PlayerDescr.push_back(pd);
}

CString CGameGUI::playGame()
{
	load();
	leave2DMode();

	_USGameCore = m_GameCore;
	m_WinSys->runLoop(game_mainloop, true); //true: swap buffers

	enter2DMode();
	unload();

	return "hiscore";
}

void CGameGUI::load()
{
	CGUIPage *page = (CGUIPage *)m_ChildWidget;
	page->m_Menu.m_Selected = 0;
	page->m_Title = "Loading";
	page->m_Menu.m_Lines.clear();
	page->m_Menu.m_Lines.push_back("please wait......");
	onRedraw();
	m_WinSys->swapBuffers();

	unload(); //just in case...

	//init game
	if(m_GameType == LocalGame)
	{
		m_GameCore->initLocalGame(m_TrackFile);
	}
	else if(m_GameType == NewNetwork)
	{
		if(m_Server != NULL)
			{delete m_Server; m_Server = NULL;}

		m_Server = new CUSServer(m_ServerPort, m_TrackFile);

		m_GameCore->initClientGame("localhost", m_ServerPort);
	}
	else if(m_GameType == JoinNetwork)
	{
		m_GameCore->initClientGame(m_HostName, m_HostPort);
	}

	//add players
	unsigned int numHumanPlayers = 0;
	for(unsigned int i=0; i < m_PlayerDescr.size(); i++)
	{
		CPlayer *p;
		CObjectChoice choice;
		choice.m_Filename = m_PlayerDescr[i].carFile;

		if(m_PlayerDescr[i].isHuman)
			p = new CHumanPlayer((CGameWinSystem *)m_WinSys, numHumanPlayers);
		else
			p = new CAIPlayerCar();

		if(!m_GameCore->addPlayer(p, m_PlayerDescr[i].name, choice))
		{
			page->m_Title = "Error:";
			page->m_Menu.m_Lines.clear();
			page->m_Menu.m_Lines.push_back("Sim doesn't accept player");
			onRedraw();
			m_WinSys->swapBuffers();
			delete p;
			continue;
		}

		if(m_PlayerDescr[i].isHuman) //set a camera to this player:
		{
			m_GameCore->addCamera(p->m_MovingObjectId);
			numHumanPlayers++;
		}

		m_Players.push_back(p);
	}

	m_GameCore->startGame();
}

void CGameGUI::unload()
{
	m_GameCore->leaveGame();

	m_Players.clear();
	for(unsigned int i=0; i<m_Players.size(); i++)
		delete m_Players[i];

	//do not yet unload the server: we might want to play again
}

CString CGameGUI::viewHiscore()
{
	CGUIPage *page = (CGUIPage *)m_ChildWidget;
	page->m_Menu.m_Selected = 0;
	page->m_Title = "Hiscore (in future versions)";
	page->m_Menu.m_Lines.clear();
	page->m_Menu.m_Lines.push_back("Play again");
	page->m_Menu.m_Lines.push_back("Return to main menu");
	m_WinSys->runLoop(this);

	switch(page->m_Menu.m_Selected)
	{
		case 0:
			return "playgame";
			break;
		case 1:
			if(m_Server != NULL)
				{delete m_Server; m_Server = NULL;} //stop the server if it's running
			return "mainmenu";
			break;
	}

	return "";
}
