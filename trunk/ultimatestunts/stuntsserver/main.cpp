/***************************************************************************
                          main.cpp  -  Stunts-Server main
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

#include <cstdlib>
#include <cstdio>

#include "gamecore.h"

#include "objectchoice.h"
#include "aiplayercar.h"

#include "lconfig.h"
#include "filecontrol.h"

vector<CPlayer *> players;
CGameCore *gamecore = NULL;

CString getInput(CString question="")
{
	printf("%s", question.c_str());
	char input[80];
	scanf("%s", input);
	return input;
}

bool addPlayer()
{
	CPlayer *p = new CAIPlayerCar();

	CString name = getInput("Enter name: ");
	printf("You entered %s\n", name.c_str());
	CString car = getInput("Enter the car: ");
	printf("You entered %s\n", car.c_str());

	CObjectChoice choice;
	choice.m_Filename = car;

	if(!gamecore->addPlayer(p, "defaultname", choice))
	{
		printf("Sim doesn't accept player\n");
		delete p;
		return false;
	}

	players.push_back(p);
	return true;
}

bool mainloop()
{
	return gamecore->update();
}

int main(int argc, char *argv[])
{
	printf("Welcome to the " PACKAGE " server version " VERSION "\n");

	int port = 1500;
	CString trackfile;
	unsigned int remote_players;
	unsigned int ai_players;

	printf("Loading configuration file\n");
	theMainConfig = new CLConfig(argc, argv);
	theMainConfig->setFilename("ultimatestunts.conf");

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

	printf("Parsing input arguments\n");
	for(int i=0; i<argc; i++)
	{
		CString arg = argv[i];
		int is = arg.inStr('=');
		if(is > 0)
		{
			CString lhs = arg.mid(0,is);
			CString rhs = arg.mid(is+1);
			if(lhs == "port")
				port = rhs.toInt();
		}
	}

	printf("---Game core\n");
	//world = new CWorld();
	gamecore = new CGameCore;

	CString answ = getInput("Do you want to use a \"super-server\" (y/n)? ");
	printf("\nYou entered %c\n", answ[0]);
	if(answ[0]=='y' || answ[0]=='Y')
	{
		CString h = getInput("Enter the super-server's hostname: ");
		int p = getInput("Enter the super-server's port number: ").toInt();

		printf("Creating client-type simulation\n");
		gamecore->initClientGame(h, p);
	}
	else
	{
		trackfile = getInput("Please enter the track file: ");
		printf("\nYou entered %s\n", trackfile.c_str());
		gamecore->initLocalGame(trackfile);
	}

	remote_players = getInput("Enter the number of remote players: ").toInt();
	printf("\nYou entered %d\n", remote_players);
	ai_players = getInput("Enter the number of AI players: ").toInt();
	printf("\nYou entered %d\n", ai_players);

	for(unsigned int i=0; i<ai_players; i++)
		addPlayer();

	gamecore->startGame();

	printf("\nEntering mainloop\n");

	//Creating some white space
	for(int i=1; i<40; i++)
		printf("\n");

	while(mainloop());

	printf("\nLeaving mainloop\n");

	printf("\n---Game core\n");
	delete gamecore;

	printf("\n---Players\n");
	for(unsigned int i=0; i<players.size(); i++)
		delete players[i];
	players.clear();

	return EXIT_SUCCESS;
}
