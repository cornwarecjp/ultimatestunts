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
#include <cstdlib>

#include "gamegui.h"
#include "menu.h"

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

	m_ChildWidget = &m_MainPage;

	//default values:
	m_GameType = LocalGame;
	m_TrackFile = "tracks/default.track";
	m_HostName = "localhost";
	m_HostPort = 1500;
	m_MaxNumPlayers = 2;
	addPlayer("CJP", true, "cars/diablo.conf");
	addPlayer("AI", false, "cars/f1.conf");

	//Load an initial state
	m_GameCore->initLocalGame(m_TrackFile);

	//setting up the menus:
	//MAIN MENU
	m_MainPage.m_Title = "Main menu";
	CMenu *menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_MainPage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = true;

	//GAMETYPE MENU
	m_GameTypePage.m_Title = "Select the game type:";
	menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_GameTypePage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = true;

	//TRACK MENU
	m_TrackPage.m_Title = "Select a track:";
	menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_TrackPage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = false;

	//PLAYER MENU
	m_PlayerPage.m_Title = "Select a car:";
	menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_PlayerPage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = false;

	//LOADING MENU
	m_LoadingPage.m_Title = "Loading";
	menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_LoadingPage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = false;
	menu->m_Lines.push_back("please wait...");

	//HISCORE MENU
	m_HiscorePage.m_Title = "Hiscore (in future versions)";
	menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_HiscorePage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = false;

	updateMenuTexts();
}

CGameGUI::~CGameGUI()
{
	delete m_GameCore;

	if(m_Server != NULL)
	{
		sleep(1);
		delete m_Server;
		m_Server = NULL;
	}
}

void CGameGUI::updateMenuTexts()
{
	//setting up the menus:
	//MAIN MENU
	CMenu *menu = (CMenu *)(m_MainPage.m_Widgets[0]);
	menu->m_Lines.clear();
	menu->m_Lines.push_back("Start playing");
	menu->m_Lines.push_back("Set the game type");
	menu->m_Lines.push_back("Select the track");
	menu->m_Lines.push_back("Select the players");
	menu->m_Lines.push_back("Options");
	menu->m_Lines.push_back("Exit");
	//add some information
	if(m_GameType == LocalGame)
	{
		menu->m_Lines[1] += CString(" [Local Game]");
		menu->m_Lines[2] += CString(" [" + m_TrackFile + "]");
	}
	else if(m_GameType == NewNetwork)
	{
		menu->m_Lines[1] += CString(" [Server started on port ") + m_HostPort + "]";
		menu->m_Lines[2] += CString(" [") + m_TrackFile + "]";
	}
	else //JoinNetwork
	{
		menu->m_Lines[1] += CString(" [Joining game at ") + m_HostName + ":" + m_HostPort + "]";
		menu->m_Lines[2] += CString(" [server will choose one]");
	}
	menu->m_Lines[3] += CString(" [") + int(m_PlayerDescr.size()) + " players selected]";


	//GAMETYPE MENU
	menu = (CMenu *)(m_GameTypePage.m_Widgets[0]);
	menu->m_Lines.clear();
	menu->m_Lines.push_back("Local game");
	menu->m_Lines.push_back("Join an existing network game");
	menu->m_Lines.push_back("Start a new network game");

	//TRACK MENU
	menu = (CMenu *)(m_TrackPage.m_Widgets[0]);
	menu->m_Lines = getDirContents("tracks", ".track");

	//PLAYER MENU
	menu = (CMenu *)(m_PlayerPage.m_Widgets[0]);
	menu->m_Lines = getDirContents("cars", ".conf");

	//HISCORE MENU
	menu = (CMenu *)(m_HiscorePage.m_Widgets[0]);
	menu->m_Lines.clear();
	menu->m_Lines.push_back("Play again");
	menu->m_Lines.push_back("Return to main menu");
}

void CGameGUI::start()
{
	enter2DMode();

	CString section = "mainmenu";
	while(section != "" && section != "exit")
	{
		updateMenuTexts();

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
	m_ChildWidget = &m_MainPage;
	m_WinSys->runLoop(this);
	CMenu *menu = (CMenu *)(m_MainPage.m_Widgets[0]);

	switch(menu->m_Selected)
	{
		case 0:
			return "playgame";
		case 1:
			return "gametypemenu";
		case 2:
			if(m_GameType == JoinNetwork)
			{
				showMessageBox("The track must be selected on the server");
				return "mainmenu";
			}
			return "trackmenu";
		case 3:
			return "playermenu";
		case 4:
			showMessageBox("Please edit ultimatestunts.conf manually");
			return "mainmenu";
		case 5:
			return "exit";
	}

	return "";
}

CString CGameGUI::viewGameTypeMenu()
{
	m_ChildWidget = &m_GameTypePage;
	m_WinSys->runLoop(this);
	CMenu *menu = (CMenu *)(m_GameTypePage.m_Widgets[0]);

	switch(menu->m_Selected)
	{
		case 0:
			m_GameType = LocalGame;
			m_GameCore->initLocalGame(m_TrackFile);
			break;
		case 1:
			m_GameType = JoinNetwork;
			m_HostName = showInputBox("Enter the host name:", m_HostName);
			m_HostPort = showInputBox("Enter the port number:", m_HostPort).toInt();
			break;
		case 2:
			m_GameType = NewNetwork;
			m_HostName = "localhost";
			m_HostPort = showInputBox("Enter the port number:", m_HostPort).toInt();
			m_MaxNumPlayers = showInputBox("Maximum number of players:", m_MaxNumPlayers).toInt();
			break;
	}

	if(m_GameType == NewNetwork)
	{
		if(m_Server == NULL)
		{
			//start server if needed
			m_Server = new CUSServer(m_HostPort, m_MaxNumPlayers);
			m_Server->set("track", m_TrackFile);
			sleep(1); //give the server some time to start
		}
		else
		{
			//update server settings
			m_Server->set("port", m_HostPort);
			m_Server->set("maxRequests", m_MaxNumPlayers);
			m_Server->set("track", m_TrackFile);
		}
	}
	else
	{
		//stop server process if needed
		if(m_Server != NULL)
			{delete m_Server; m_Server = NULL;}
	}

	if(m_GameType != LocalGame)
	{
		//connect to the server
		m_GameCore->initClientGame(m_HostName, m_HostPort);
	}

	return "mainmenu";
}

CString CGameGUI::viewTrackMenu()
{
	m_ChildWidget = &m_TrackPage;
	m_WinSys->runLoop(this);
	CMenu *menu = (CMenu *)(m_TrackPage.m_Widgets[0]);

	m_TrackFile = CString("tracks/") + menu->m_Lines[menu->m_Selected];

	if(m_GameType == LocalGame)
	{
		m_GameCore->initLocalGame(m_TrackFile);
	}
	else if(m_GameType == NewNetwork && m_Server != NULL)
	{
		m_Server->set("track", m_TrackFile);
	}

	return "mainmenu";
}

CString CGameGUI::viewPlayerMenu()
{
	m_PlayerPage.m_Title = "Player setup";
	m_ChildWidget = &m_PlayerPage;
	CMenu *menu = (CMenu *)(m_PlayerPage.m_Widgets[0]);

	m_PlayerDescr.clear();
	CString name = showInputBox("Enter your name:");

	m_PlayerPage.m_Title = CString("Select a car for ") + name;
	m_WinSys->runLoop(this);
	CString carfile = CString("cars/") + menu->m_Lines[menu->m_Selected];
	addPlayer(name, true, carfile);

	while(true)
	{
		if(!showYNMessageBox("Do you want to add another player?")) break;

		name = showInputBox("Enter the name:");

		m_PlayerPage.m_Title = CString("Select a car for ") + name;
		m_WinSys->runLoop(this);
		carfile = CString("cars/") + menu->m_Lines[menu->m_Selected];

		bool isHuman = !showYNMessageBox("Is it an AI player?"); 
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
	m_ChildWidget = &m_LoadingPage;
	onResize(0, 0, m_W, m_H);
	onRedraw();
	m_WinSys->swapBuffers();

	//add players
	unsigned int numHumanPlayers = 0;
	for(unsigned int i=0; i < m_PlayerDescr.size(); i++)
	{
		if(m_GameType == NewNetwork && !(m_PlayerDescr[i].isHuman))
			continue; //AI players will be added on the server side

		CPlayer *p;
		CObjectChoice choice;
		choice.m_Filename = m_PlayerDescr[i].carFile;

		if(m_PlayerDescr[i].isHuman)
			p = new CHumanPlayer((CGameWinSystem *)m_WinSys, numHumanPlayers);
		else
			p = new CAIPlayerCar();

		if(!m_GameCore->addPlayer(p, m_PlayerDescr[i].name, choice))
		{
			showMessageBox(CString("Player ") + m_PlayerDescr[i].name + " was refused");
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

	if(m_GameType == NewNetwork)
	{
		//In a new network game, place the AI's on the server side
		for(unsigned int i=0; i < m_PlayerDescr.size(); i++)
			if(!(m_PlayerDescr[i].isHuman))
				m_Server->addai(m_PlayerDescr[i].name, m_PlayerDescr[i].carFile);

		sleep(1); //To give messages some time to arive on server (TODO: remove when no longer needed)

		//start the game on the server side
		m_Server->start();
	}

	m_GameCore->readyAndLoad();
}

void CGameGUI::unload()
{
	m_GameCore->resetGame();

	if(m_GameType == NewNetwork && m_Server != NULL)
		m_Server->stop();

	for(unsigned int i=0; i<m_Players.size(); i++)
		delete m_Players[i];
	m_Players.clear();

	//do not yet unload the server: we might want to play again
}

CString CGameGUI::viewHiscore()
{
	m_ChildWidget = &m_HiscorePage;
	m_WinSys->runLoop(this);
	CMenu *menu = (CMenu *)m_HiscorePage.m_Widgets[0];

	switch(menu->m_Selected)
	{
		case 0:
			return "playgame";
			break;
		case 1:
			return "mainmenu";
			break;
	}

	return "";
}
