/***************************************************************************
                          main.cpp  -  UltimateStunts main
                             -------------------
    begin                : wo nov 20 23:23:28 CET 2002
    copyright            : (C) 2002 by CJP
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//Standard includes
#include <cstdlib>
#include <cstdio>
#include <vector>
namespace std {}
using namespace std;

//Frequently used
#include "cstring.h"
#include "lconfig.h"
#include "filecontrol.h"
#include "usmacros.h"

//Simulation stuff
#include "car.h"

#include "usserver.h"

//Player stuff
#include "objectchoice.h"
#include "aiplayercar.h"
#include "humanplayer.h"

#include "gamegui.h"

#include "uscore.h"

CUSCore *gamecore = NULL;
CGameGUI *gui = NULL;

vector<CPlayer *> players;
unsigned int numHumanPlayers = 0;

CUSServer *server = NULL;

CGameWinSystem *winsys = NULL;

bool mainloop()
{
	return gamecore->update();
}

bool addPlayer(bool isHuman, CString name, CObjectChoice choice)
{
	CPlayer *p;

	if(isHuman)
		p = new CHumanPlayer(winsys, numHumanPlayers);
	else
		p = new CAIPlayerCar();

	if(!gamecore->addPlayer(p, name, choice))
	{
		printf("Sim doesn't accept player\n");
		delete p;
		return false;
	}

	if(isHuman) //set a camera to this player:
	{
		gamecore->addCamera(p->m_MovingObjectId);
		numHumanPlayers++;
	}

	players.push_back(p);
	return true;
}

void start(int argc, char *argv[]) //first things before becoming interactive
{
	printf("Welcome to " PACKAGE " version " VERSION "\n");

	theMainConfig = new CLConfig(argc, argv);
	if(!theMainConfig->setFilename("ultimatestunts.conf"))
	{
		printf("Error: could not read ultimatestunts.conf\n");
		//TODO: create a default one
	} else {printf("Using ultimatestunts.conf\n");}

	CFileControl *fctl = new CFileControl;
	{
		//Default:
		CString DataDir = ""; //try in default directories, like "./"

		CString cnf = theMainConfig->getValue("files", "datadir");
		if(cnf != "")
		{
			if(cnf[cnf.length()-1] != '/') cnf += '/';
			DataDir = cnf;
		}
		fctl->setDataDir(DataDir);
	}

	printf("Initialising Ultimate Stunts:\n---Window system\n");
	winsys = new CGameWinSystem("Ultimate Stunts", *theMainConfig);

	printf("---GUI\n");
	gui = new CGameGUI(*theMainConfig, winsys);

	printf("---Game core\n");
	gamecore = new CUSCore(winsys);
}

void end() //Last things before exiting
{
	printf("Unloading Ultimate Stunts:\n\n");
	
	printf("---Game core\n");
	if(gamecore != NULL)
		delete gamecore;

	printf("---Players\n");
	for(unsigned int i=0; i<players.size(); i++)
		delete players[i];
	players.clear();

	printf("---Server\n");
	if(server!=NULL)
		delete server; //also stops the server process (if existing)

	printf("---GUI\n");
	if(gui!=NULL)
		delete gui;

	printf("---Window system\n");
	if(winsys!=NULL)
		delete winsys; //Important; don't remove: this calls SDL_Quit!!!

	printf("\nProgram finished succesfully\n");
}

int main(int argc, char *argv[])
{
	start(argc, argv);

	//The track filename:
	CString trackfile = "tracks/default.track";

	//MENU SECTION:
	gui->startFrom("mainmenu");
	while(!( gui->isPassed("mainmenu") )); //waiting for input
	CGameGUI::eMainMenu maininput =
		(CGameGUI::eMainMenu)gui->getValue("mainmenu").toInt();

	switch(maininput)
	{
		case CGameGUI::LocalGame:
			while(!( gui->isPassed("trackmenu") )); //waiting for input
			gamecore->initLocalGame(gui->getValue("trackmenu"));
			break;
		case CGameGUI::NewNetwork:
		{
			//Server menu:
			while(!( gui->isPassed("servermenu") )); //waiting for input

			//Track menu:
			while(!( gui->isPassed("trackmenu") )); //waiting for input

			CString name = "localhost";
			int port = gui->getValue("servermenu", "portnumber").toInt();

			trackfile = gui->getValue("trackmenu");

			server = new CUSServer(port, trackfile);
			printf("Creating client-type simulation with %s:%d\n", name.c_str(), port);

			gamecore->initClientGame(name, port);
			break;
		}
		case CGameGUI::JoinNetwork:
		{
			while(!( gui->isPassed("hostmenu") )); //waiting for input
			CString name = gui->getValue("hostmenu", "hostname");
			int port = gui->getValue("hostmenu", "portnumber").toInt();
			printf("Creating client-type simulation with %s:%d\n", name.c_str(), port);
			gamecore->initClientGame(name, port);
			break;
		}
		default:
		case CGameGUI::Exit:
			end();
			return EXIT_SUCCESS;
	}

	//Set up the players
	while(!( gui->isPassed("playermenu") )); //waiting for input
	int num_players = gui->getValue("playermenu", "number").toInt();
	printf("\nNumber of players to be added: %d\n", num_players);
	for(int i=0; i<num_players; i++)
	{
		CString name = gui->getValue("playermenu", CString("name ") + i);
		bool isHuman = gui->getValue("playermenu", CString("ishuman ") + i) == "true";
		CString carFile = gui->getValue("playermenu", CString("carfile ") + i);
		CObjectChoice oc;
		oc.m_Filename = carFile;
		printf("%d: \"%s\" driving %s\n", i, name.c_str(), carFile.c_str());
		addPlayer(isHuman, name, oc);
	}

	printf("\nStarting the game\n");
	gamecore->startGame();

	printf("\nEntering mainloop\n");
	//Creating some white space
	for(int i=1; i<40; i++)
		printf("\n");
	winsys->runLoop(mainloop, true); //true: swap buffers
	printf("\nLeaving mainloop\n");

	end();
	return EXIT_SUCCESS;
}
