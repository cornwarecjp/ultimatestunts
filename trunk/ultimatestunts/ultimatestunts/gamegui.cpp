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

#include "objectchoice.h"
#include "aiplayercar.h"
#include "humanplayer.h"

CUSCore *_USGameCore;

bool game_mainloop()
{
	return _USGameCore->update();
}


CGameGUI::CGameGUI(const CLConfig &conf, CGameWinSystem *winsys) : CGUI(conf, winsys)
{
	m_GameCore = new CUSCore(winsys);

	m_Server = NULL;

	//default values:
	m_TrackFile = "tracks/default.track";
	addPlayer("CJP", true, "cars/diablo.conf");
	addPlayer("AI", false, "cars/f1.conf");
}

CGameGUI::~CGameGUI()
{
	delete m_GameCore;

	if(m_Server != NULL)
		{delete m_Server; m_Server = NULL;}
}

void CGameGUI::start()
{
	CString section = "mainmenu";
	while(section != "" && section != "exit")
	{
		printf("\n\n"); //create some space

		if(section=="mainmenu")
			section = viewMainMenu();
		else if(section=="hostmenu")
			section = viewHostMenu();
		else if(section=="servermenu")
			section = viewServerMenu();
		else if(section=="trackmenu")
			section = viewTrackMenu();
		else if(section=="playermenu")
			section = viewPlayerMenu();
		else if(section=="playgame")
			section = playGame();
		else if(section=="hiscore")
			section = viewHiscore();
		else
			{printf("Error: unknown menu %s\n", section.c_str()); section = "";}
	}
}

CString CGameGUI::viewMainMenu()
{
	CString ret = "incorrect_answer";
	while(ret == "incorrect_answer")
	{
		printf(
			"Main menu:\n"
			"  1: Play a local game\n"
			"  2: Start a new network game\n"
			"  3: Log in on a remote network game\n"
			"  \n"
			"  4: Exit\n"
		);
		int input = getInput().toInt();

		switch(input)
		{
			case 1:
				m_MainMenuInput = LocalGame;
				ret = "trackmenu";
				break;
			case 2:
				m_MainMenuInput = NewNetwork;
				ret = "servermenu";
				break;
			case 3:
				m_MainMenuInput = JoinNetwork;
				ret = "hostmenu";
				break;
			case 4:
				m_MainMenuInput = Exit;
				ret = "exit";
				break;
			default:
				printf("Incorrect answer\n");
		}

	} //while

	return ret;
}

CString CGameGUI::viewServerMenu()
{
	printf("Server menu:\n");
	printf("Enter the port number: ");
	m_ServerPort = getInput().toInt();

	return "trackmenu";
}

CString CGameGUI::viewHostMenu()
{
	printf(
		"Host menu:\n"
		"Enter the server's host name: "
	);
	m_HostName = getInput();
	printf("Enter the server's UDP port number: ");
	m_HostPort = getInput().toInt();

	return "playermenu";
}

CString CGameGUI::viewTrackMenu()
{
	CString defaulttrack = "tracks/default.track";
	printf("Track menu:\n");
	printf("Enter the track filename (default is %s): ", defaulttrack.c_str());
	m_TrackFile = getInput();
	if(m_TrackFile == "") m_TrackFile = defaulttrack;

	return "playermenu";
}

CString CGameGUI::viewPlayerMenu()
{
	CString defaultcar = "cars/diablo.conf";
	m_PlayerDescr.clear();

	printf("Player menu:\n");
	printf("Enter your name: ");
	CString name = getInput();
	printf("Enter your car file (default is %s):", defaultcar.c_str());
	CString carfile = getInput();
	if(carfile == "") carfile = defaultcar;
	addPlayer(name, true, carfile);

	while(true)
	{
		printf("\nDo you want to add a player(y/n)? ");
		CString answ = getInput();
		if(!(answ == "y" || answ == "Y")) break;

		printf("Enter the name: ");
		name = getInput();
		printf("Enter the car file (default is %s):", defaultcar.c_str());
		carfile = getInput();
		if(carfile == "") carfile = defaultcar;
		printf("Is it an AI player(y/n)? ");
		answ = getInput();
		bool isHuman = !(answ == "y" || answ == "Y");
		addPlayer(name, isHuman, carfile);
	}

	return "playgame";
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

	//white space between loading and game messages
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

	_USGameCore = m_GameCore;
	m_WinSys->runLoop(game_mainloop, true); //true: swap buffers

	printf("\n\n\n");

	unload();

	return "hiscore";
}

void CGameGUI::load()
{
	unload(); //just in case...

	//init game
	if(m_MainMenuInput == LocalGame)
	{
		m_GameCore->initLocalGame(m_TrackFile);
	}
	else if(m_MainMenuInput == NewNetwork)
	{
		if(m_Server != NULL)
			{delete m_Server; m_Server = NULL;}

		m_Server = new CUSServer(m_ServerPort, m_TrackFile);

		m_GameCore->initClientGame("localhost", m_ServerPort);
	}
	else if(m_MainMenuInput == JoinNetwork)
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
			printf("Sim doesn't accept player\n");
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
	CString ret = "incorrect_answer";
	while(ret == "incorrect_answer")
	{
		printf(
			"In the future, here will be the high scores\n"
			"  1: Play again\n"
			"  2: Return to main menu\n"
		);
		int input = getInput().toInt();

		switch(input)
		{
			case 1:
				ret = "playgame";
				break;
			case 2:
				if(m_Server != NULL)
					{delete m_Server; m_Server = NULL;} //stop the server if it's running
				ret = "mainmenu";
				break;
			default:
				printf("Incorrect answer\n");
		}

	} //while

	return ret;
}
