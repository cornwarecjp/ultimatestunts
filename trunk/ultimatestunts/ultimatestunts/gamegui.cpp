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
#include <unistd.h>

#include <libintl.h>
#define _(String) gettext (String)
#define N_(String1, String2, n) ngettext ((String1), (String2), (n))

#include "lconfig.h"

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

	//Car file cache:
	vector<CString> theCarFiles = getDirContents("cars", ".conf");
	for(unsigned int i=0; i < theCarFiles.size(); i++)
	{
		SCarFile cf;
		cf.filename = "cars/" + theCarFiles[i];
		CDataFile dfile(cf.filename);
		CLConfig carconf(dfile.useExtern());
		cf.fullname = carconf.getValue("description", "fullname");
		//printf("file %s name %s\n", cf.filename.c_str(), cf.fullname.c_str());
		m_CarFiles.push_back(cf);
	}

	//default values:
	m_GameType = LocalGame;
	m_TrackFile = "tracks/default.track";
	m_HostName = "localhost";
	m_HostPort = 1500;
	m_MaxNumPlayers = 2;

	//Default players:
	SPlayerDescr pd;
	pd.name = "CJP";
	pd.isHuman = true;
	for(unsigned int i=0; i < m_CarFiles.size(); i++)
		if(m_CarFiles[i].filename == "cars/ferrarispider.conf")
		{
			pd.carIndex = i;
			break;
		}
	m_PlayerDescr.push_back(pd);
	m_SelectedPlayer = 0;

	//Load an initial state
	m_GameCore->initLocalGame(m_TrackFile);

	//setting up the menus:
	//MAIN MENU
	m_MainPage.m_Title = _("Main menu");
	CMenu *menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_MainPage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = true;

	//GAMETYPE MENU
	m_GameTypePage.m_Title = _("Select the game type:");
	menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_GameTypePage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = true;

	//TRACK MENU
	m_TrackPage.m_Title = _("Select a track:");
	menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_TrackPage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = false;

	//PLAYERS MENU
	m_PlayersPage.m_Title = _("Players menu:");
	menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_PlayersPage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = true;

	//PLAYER MENU
	m_PlayerPage.m_Title = _("Configure Player:");
	menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_PlayerPage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = true;

	//CAR MENU
	m_CarPage.m_Title = "Select a car:";
	menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_CarPage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = false;

	//LOADING MENU
	m_LoadingPage.m_Title = _("Loading");
	menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_LoadingPage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = false;
	menu->m_Lines.push_back(_("please wait..."));

	//HISCORE MENU
	menu = new CMenu;
	menu->m_Xrel = 0.05;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.3;
	menu->m_Hrel = 0.6;
	menu->m_Selected = 21;
	menu->m_AlignLeft = true;
	m_HiscorePage.m_Widgets.push_back(menu);

	menu = new CMenu;
	menu->m_Xrel = 0.35;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.3;
	menu->m_Hrel = 0.6;
	menu->m_Selected = 21;
	menu->m_AlignLeft = true;
	m_HiscorePage.m_Widgets.push_back(menu);

	menu = new CMenu;
	menu->m_Xrel = 0.65;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.3;
	menu->m_Hrel = 0.6;
	menu->m_Selected = 21;
	menu->m_AlignLeft = true;
	m_HiscorePage.m_Widgets.push_back(menu);

	m_HiscorePage.m_Title = _("Hiscore");
	menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.1;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.1;
	menu->m_Selected = 0;
	menu->m_AlignLeft = false;
	m_HiscorePage.m_Widgets.push_back(menu);

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
	//some texts that are used more than one time:
	CString txtPlayertype[2];
	txtPlayertype[0] = _("Computer");
	txtPlayertype[1] = _("Human   ");

	//setting up the menus:
	//MAIN MENU
	CMenu *menu = (CMenu *)(m_MainPage.m_Widgets[0]);
	menu->m_Lines.clear();
	menu->m_Lines.push_back(_("Drive!"));
	menu->m_Lines.push_back(_("Set the game type"));
	menu->m_Lines.push_back(_("Select the track"));
	menu->m_Lines.push_back(_("Select the players"));
	menu->m_Lines.push_back(_("Options"));
	menu->m_Lines.push_back(_("Exit"));
	//add some information
	if(m_GameType == LocalGame)
	{
		menu->m_Lines[1] += CString(_(" [Local Game]"));
		menu->m_Lines[2] += CString(" [" + m_TrackFile + "]");
	}
	else if(m_GameType == NewNetwork)
	{
		menu->m_Lines[1] += CString().format(_(" [Server started on port %d]"), 80, m_HostPort);
		menu->m_Lines[2] += CString(" [") + m_TrackFile + "]";
	}
	else //JoinNetwork
	{
		menu->m_Lines[1] += CString().format(_(" [Joining game at %s:%d]"), 80, m_HostName.c_str(), m_HostPort);
		menu->m_Lines[2] += CString(_(" [server will choose one]"));
	}

	int nump = int(m_PlayerDescr.size());
	menu->m_Lines[3] += CString().format(
		N_(" [%d player selected]", " [%d players selected]", nump),
		80, nump);


	//GAMETYPE MENU
	menu = (CMenu *)(m_GameTypePage.m_Widgets[0]);
	menu->m_Lines.clear();
	menu->m_Lines.push_back(_("Local game"));
	menu->m_Lines.push_back(_("Join an existing network game"));
	menu->m_Lines.push_back(_("Start a new network game"));

	//TRACK MENU
	menu = (CMenu *)(m_TrackPage.m_Widgets[0]);
	menu->m_Lines = getDirContents("tracks", ".track");

	//PLAYERS MENU
	menu = (CMenu *)(m_PlayersPage.m_Widgets[0]);
	menu->m_Lines.clear();
	for(unsigned int i=0; i < m_PlayerDescr.size(); i++)
	{
		CString type;
		CString entry =
			CString(i+1) + ": (" +
			txtPlayertype[m_PlayerDescr[i].isHuman] + ") " +
			m_PlayerDescr[i].name.forceLength(20) + ": " +
			m_CarFiles[m_PlayerDescr[i].carIndex].fullname;
		menu->m_Lines.push_back(entry);
	}
	menu->m_Lines.push_back(_("Add a player"));
	menu->m_Lines.push_back(_("Return to main menu"));

	//PLAYER MENU
	menu = (CMenu *)(m_PlayerPage.m_Widgets[0]);
	menu->m_Lines.clear();
	menu->m_Lines.push_back(_("Player type: ") + txtPlayertype[m_PlayerDescr[m_SelectedPlayer].isHuman]);
	menu->m_Lines.push_back(_("Name       : ") + m_PlayerDescr[m_SelectedPlayer].name);
	menu->m_Lines.push_back(_("Car        : ") + m_CarFiles[m_PlayerDescr[m_SelectedPlayer].carIndex].fullname);
	menu->m_Lines.push_back(_("Delete this player"));
	menu->m_Lines.push_back(_("Ready"));

	//CAR MENU
	m_CarPage.m_Title.format(_("Select a car for %s:"), 256, m_PlayerDescr[m_SelectedPlayer].name.c_str());
	menu = (CMenu *)(m_CarPage.m_Widgets[0]);
	menu->m_Lines.clear();
	for(unsigned int i=0; i < m_CarFiles.size(); i++)
		menu->m_Lines.push_back(m_CarFiles[i].fullname);

	//HISCORE MENU
	menu = (CMenu *)(m_HiscorePage.m_Widgets[3]);
	menu->m_Lines.clear();
	menu->m_Lines.push_back(_("Play again"));
	menu->m_Lines.push_back(_("Return to main menu"));
}

void CGameGUI::start()
{
	enter2DMode();

	CString section = "mainmenu";
	while(true)
	{
		updateMenuTexts();

		if(section=="mainmenu")
			section = viewMainMenu();
		else if(section=="gametypemenu")
			section = viewGameTypeMenu();
		else if(section=="trackmenu")
			section = viewTrackMenu();
		else if(section=="playersmenu")
			section = viewPlayersMenu();
		else if(section=="playermenu")
			section = viewPlayerMenu();
		else if(section=="carmenu")
			section = viewCarMenu();
		else if(section=="playgame")
			section = playGame();
		else if(section=="hiscore")
			section = viewHiscore();
		else
			{printf("Error: unknown menu\n");}

		if(section == "exit" || section == "")
			if(showYNMessageBox(_("Do you really want to quit?")) )
				{break;}
			else
				{section = "mainmenu";}
	}

	leave2DMode();
}

CString CGameGUI::viewMainMenu()
{
	m_ChildWidget = &m_MainPage;
	if(!m_WinSys->runLoop(this))
		return "exit";


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
				showMessageBox(_("The track must be selected on the server"));
				return "mainmenu";
			}
			return "trackmenu";
		case 3:
			return "playersmenu";
		case 4:
			showMessageBox(_("Please edit ultimatestunts.conf manually"));
			return "mainmenu";
		case 5:
			return "exit";
	}

	return "";
}

CString CGameGUI::viewGameTypeMenu()
{
	m_ChildWidget = &m_GameTypePage;
	if(!m_WinSys->runLoop(this))
		return "mainmenu";


	CMenu *menu = (CMenu *)(m_GameTypePage.m_Widgets[0]);

	switch(menu->m_Selected)
	{
		case 0:
			m_GameType = LocalGame;
			m_GameCore->initLocalGame(m_TrackFile);
			break;
		case 1:
			m_GameType = JoinNetwork;
			m_HostName = showInputBox(_("Enter the host name:"), m_HostName);
			m_HostPort = showInputBox(_("Enter the port number:"), m_HostPort).toInt();
			break;
		case 2:
			m_GameType = NewNetwork;
			m_HostName = "localhost";
			m_HostPort = showInputBox(_("Enter the port number:"), m_HostPort).toInt();
			m_MaxNumPlayers = showInputBox(_("Maximum number of players:"), m_MaxNumPlayers).toInt();
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
	if(!m_WinSys->runLoop(this))
		return "mainmenu";


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

CString CGameGUI::viewPlayersMenu()
{
	m_ChildWidget = &m_PlayersPage;
	if(!m_WinSys->runLoop(this)) //cancelling gives false
		return "mainmenu";

	//The rest is executed when not cancelling

	CMenu *menu = (CMenu *)(m_PlayersPage.m_Widgets[0]);

	//menu->m_Lines[menu->m_Selected]
	unsigned int tomain = menu->m_Lines.size()-1;
	unsigned int addplayer = menu->m_Lines.size()-2;
	unsigned int selected = menu->m_Selected;

	if(selected == tomain) return "mainmenu";

	if(selected == addplayer)
	{
		SPlayerDescr newpl;
		newpl.name = "AI Player";
		newpl.isHuman = false;
		for(unsigned int i=0; i < m_CarFiles.size(); i++)
			if(m_CarFiles[i].filename == "cars/ferrarispider.conf")
			{
				newpl.carIndex = i;
				break;
			}
		m_PlayerDescr.push_back(newpl);
	}

	m_SelectedPlayer = selected;
	return "playermenu";
}

CString CGameGUI::viewPlayerMenu()
{
	m_ChildWidget = &m_PlayerPage;
	if(!m_WinSys->runLoop(this)) //cancelling returns false
		return "playersmenu";

	//the rest is done when there is no cancelling

	CMenu *menu = (CMenu *)(m_PlayerPage.m_Widgets[0]);

	switch(menu->m_Selected)
	{
	case 0: //Player type
		m_PlayerDescr[m_SelectedPlayer].isHuman = !showYNMessageBox(_("Is this player an AI computer player?"));
		return "playermenu";
	case 1: //name
		m_PlayerDescr[m_SelectedPlayer].name = showInputBox(_("Enter the name:"));
		return "playermenu";
	case 2: //car
		return "carmenu";
	case 3: //delete
		if(showYNMessageBox(_("Delete this player?")) )
		{
			m_PlayerDescr.erase(m_PlayerDescr.begin() + m_SelectedPlayer);
			m_SelectedPlayer = 0;
			return "playersmenu";
		}
		return "playermenu";
	case 4: //return
		return "playersmenu";
	}

	return "playersmenu";
}

CString CGameGUI::viewCarMenu()
{
	m_ChildWidget = &m_CarPage;
	if(m_WinSys->runLoop(this))
	{
		CMenu *menu = (CMenu *)(m_CarPage.m_Widgets[0]);
		m_PlayerDescr[m_SelectedPlayer].carIndex = menu->m_Selected;
	}

	return "playermenu";
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

		//TODO: maybe use CObjectChoice class for m_PlayerDescr array
		CPlayer *p;
		CObjectChoice choice;
		choice.m_Filename = m_CarFiles[m_PlayerDescr[i].carIndex].filename;
		choice.m_PlayerName = m_PlayerDescr[i].name;

		if(m_PlayerDescr[i].isHuman)
			p = new CHumanPlayer((CGameWinSystem *)m_WinSys, numHumanPlayers);
		else
			p = new CAIPlayerCar();

		if(!m_GameCore->addPlayer(p, choice))
		{
			showMessageBox(
				CString().format(_("Player %s was refused"), 80,
				m_PlayerDescr[i].name.c_str())
				);
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
		//clear the AI list
		m_Server->clearai();

		//In a new network game, place the AI's on the server side
		for(unsigned int i=0; i < m_PlayerDescr.size(); i++)
			if(!(m_PlayerDescr[i].isHuman))
				m_Server->addai(m_PlayerDescr[i].name, m_CarFiles[m_PlayerDescr[i].carIndex].filename);

		sleep(1); //To give messages some time to arive on server (TODO: remove when no longer needed)

		//start the game on the server side
		m_Server->start();
	}

	m_GameCore->readyAndLoad();
}

void CGameGUI::unload()
{
	m_GameCore->stopGame();

	if(m_Server != NULL)
		m_Server->stop();

	for(unsigned int i=0; i<m_Players.size(); i++)
		delete m_Players[i];
	m_Players.clear();

	if(m_Server != NULL)
	{
		//clear the AI list
		m_Server->clearai();
	}

	//do not yet unload the server: we might want to play again
}

CString CGameGUI::viewHiscore()
{
	vector<SHiscoreEntry> hiscore = m_GameCore->getHiscore();

	//add hiscore data to the menu
	CMenu *names = (CMenu *)m_HiscorePage.m_Widgets[0];
	CMenu *cars = (CMenu *)m_HiscorePage.m_Widgets[1];
	CMenu *times = (CMenu *)m_HiscorePage.m_Widgets[2];
	names->m_Lines.clear();
	cars->m_Lines.clear();
	times->m_Lines.clear();
	for(unsigned int i=0; i < hiscore.size(); i++)
	{
		CString name =
			CString().format("%d ", 8, i+1) +
			hiscore[i].name + "  ....................";
		CString car = CString(" ") + hiscore[i].carname + "  ....................";
		CString time = CString().fromTime(hiscore[i].time);

		if(hiscore[i].isNew) time += _("  (this race)");

		names->m_Lines.push_back(name);
		cars->m_Lines.push_back(car);
		times->m_Lines.push_back(time);
	}

	m_ChildWidget = &m_HiscorePage;
	if(!m_WinSys->runLoop(this))
		return "mainmenu";


	CMenu *menu = (CMenu *)m_HiscorePage.m_Widgets[3];

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
