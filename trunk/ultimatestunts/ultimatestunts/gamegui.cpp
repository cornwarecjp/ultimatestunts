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

#include "datafile.h"

CUSCore *_USGameCore;

bool game_mainloop()
{
	return _USGameCore->update();
}


CGameGUI::CGameGUI(const CLConfig &conf, CGameWinSystem *winsys) : CGUI(conf, winsys)
{
	m_GameCore = new CUSCore(winsys);

	m_Server = NULL;

	m_Console = new CConsole(winsys);

	//default values: (currently unused)
	m_TrackFile = "tracks/default.track";
	addPlayer("CJP", true, "cars/diablo.conf");
	addPlayer("AI", false, "cars/f1.conf");
}

CGameGUI::~CGameGUI()
{
	delete m_GameCore;

	if(m_Server != NULL)
		{delete m_Server; m_Server = NULL;}

	delete m_Console;
}

void CGameGUI::start()
{
	m_Console->clearScreen();

	CString section = "mainmenu";
	while(section != "" && section != "exit")
	{
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
			{m_Console->print(CString("Error: unknown menu ") + section.c_str() + "\n"); section = "";}
	}
}

CString CGameGUI::viewMainMenu()
{
	CString ret = "incorrect_answer";
	while(ret == "incorrect_answer")
	{
		int input = m_Console->getInput(
			"Main menu:\n"
			"  1: Play a local game\n"
			"  2: Start a new network game\n"
			"  3: Log in on a remote network game\n"
			"  \n"
			"  4: Exit\n: "
		).toInt();

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
				m_Console->print("Incorrect answer\n");
		}

	} //while

	return ret;
}

CString CGameGUI::viewServerMenu()
{
	m_Console->print("Server menu:\n");
	m_ServerPort = m_Console->getInput("Enter the port number: ").toInt();

	return "trackmenu";
}

CString CGameGUI::viewHostMenu()
{
	m_Console->print("Host menu:\n");
	m_HostName = m_Console->getInput("Enter the server's host name: ");
	m_HostPort = m_Console->getInput("Enter the server's UDP port number: ").toInt();

	return "playermenu";
}

CString CGameGUI::viewTrackMenu()
{
	m_Console->print("\nTrack menu.");

	m_TrackFile = "";
	while(m_TrackFile == "")
	{
		m_Console->print("Choose a track from:");

		//list the tracks
		vector<CString> tracks = getDirContents("tracks", ".track");
		for(unsigned int i=0; i < tracks.size(); i++)
			m_Console->print(CString((int)i+1) + ": " + tracks[i]);

		int i = m_Console->getInput("Which track? ").toInt() - 1;
		if(i >= 0 && i < (int)(tracks.size()))
			m_TrackFile = CString("tracks/") + tracks[i];

	}

	return "playermenu";
}

CString CGameGUI::viewPlayerMenu()
{
	CString defaultcar = "diablo.conf";
	m_PlayerDescr.clear();

	m_Console->print("Player menu:\n");
	CString name = m_Console->getInput("Enter your name: ");

	CString carfile = "";
	while(carfile == "")
	{
		m_Console->print("Choose a car from:");

		//list the tracks
		vector<CString> cars = getDirContents("cars", ".conf");
		for(unsigned int i=0; i < cars.size(); i++)
			m_Console->print(CString((int)i+1) + ": " + cars[i]);

		int i = m_Console->getInput("Which car? ").toInt() - 1;
		if(i >= 0 && i < (int)(cars.size()))
			carfile = CString("cars/") + cars[i];

	}

	addPlayer(name, true, carfile);

	while(true)
	{
		CString answ = m_Console->getInput("\nDo you want to add a player(y/n)? ");
		if(!(answ == "y" || answ == "Y")) break;

		name = m_Console->getInput("Enter the name: ");

		carfile = "";
		while(carfile == "")
		{
			m_Console->print("Choose a car from:");

			//list the tracks
			vector<CString> cars = getDirContents("cars", ".conf");
			for(unsigned int i=0; i < cars.size(); i++)
				m_Console->print(CString((int)i+1) + ": " + cars[i]);

			int i = m_Console->getInput("Which car? ").toInt() - 1;
			if(i >= 0 && i < (int)(cars.size()))
				carfile = CString("cars/") + cars[i];

		}

		answ = m_Console->getInput("Is it an AI player(y/n)? ");
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

	_USGameCore = m_GameCore;
	m_WinSys->runLoop(game_mainloop, true); //true: swap buffers

	unload();

	return "hiscore";
}

void CGameGUI::load()
{
	m_Console->print("Loading, please wait......");
	m_Console->clearScreen();
	m_Console->draw();
	m_WinSys->swapBuffers();

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
			m_Console->print("Sim doesn't accept player\n");
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
		int input = m_Console->getInput(
			"In the future, here will be the high scores\n"
			"  1: Play again\n"
			"  2: Return to main menu\n"
		).toInt();

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
				m_Console->print("Incorrect answer\n");
		}

	} //while

	return ret;
}
