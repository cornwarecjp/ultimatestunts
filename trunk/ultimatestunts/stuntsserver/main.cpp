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

#include <vector>
namespace std {}
using namespace std;

#include "gamecorethread.h"
#include "networkthread.h"

#include "objectchoice.h"
#include "aiplayercar.h"
#include "chatmessage.h"

#include "lconfig.h"
#include "filecontrol.h"

#include "client.h"

//The game objects
vector<CPlayer *> players;
CGamecoreThread gamecorethread;
CNetworkThread networkthread;

CClientList Clients;

//The variables
CString Trackfile = "tracks/default.track";
unsigned int remoteplayers = 2;

struct SAIDescr
{
	CString name;
	CString carFile;
};
vector<SAIDescr> AIDescriptions;



CString getInput(CString question="")
{
	printf("%s", question.c_str());
	CString out;
	char input[80];
	while(true)
	{
		scanf("%s", input);
		out += input;
		char c = getchar();
		if(c == '\n') break;
		out += c;
	}
	return out;
}

void addPlayers()
{
	//AI players:
	for(unsigned int i=0; i<AIDescriptions.size(); i++)
	{
		CString name = AIDescriptions[i].name;
		CString car = AIDescriptions[i].carFile;

		CPlayer *p = new CAIPlayerCar();

		CObjectChoice choice;
		choice.m_Filename = car;

		if(!gamecorethread.m_GameCore->addPlayer(p, name, choice))
		{
			printf("Sim doesn't accept AI player %s\n", name.c_str());
			delete p;
			continue;
		}

		players.push_back(p);
	}
}

void cmd_start()
{
	//TODO: super-server setup
	gamecorethread.m_GameCore->initLocalGame(Trackfile);

	addPlayers();
	gamecorethread.m_GameCore->readyAndLoad();

	printf("Starting gamecore thread\n");
	if(!gamecorethread.start())
		printf("For some reason, starting the thread failed\n");
}

void cmd_stop()
{
	printf("Stopping gamecore thread\n");
	if(!gamecorethread.stop())
		printf("For some reason, stopping the thread failed\n");
}

void cmd_exit()
{
	printf("\n---Players\n");
	for(unsigned int i=0; i<players.size(); i++)
		delete players[i];
	players.clear();

	printf("\n---Gamecore\n");
	delete gamecorethread.m_GameCore;
	
	exit(EXIT_SUCCESS);
}

void cmd_addai(const CString &args)
{
	int pos = args.inStr(',');
	if(pos > 0)
	{
		CString name = args.mid(0, pos);
		CString car = args.mid(pos+1);
		name.Trim();
		car.Trim();

		printf("Adding AI \'%s\' with car \'%s\'\n", name.c_str(), car.c_str());
		SAIDescr ai;
		ai.name = name;
		ai.carFile = car;
		AIDescriptions.push_back(ai);
	}
}

void cmd_set(const CString &args)
{
	int pos = args.inStr('=');
	if(pos > 0)
	{
		CString var = args.mid(0, pos);
		CString val = args.mid(pos+1);
		var.Trim();
		val.Trim();

		printf("Setting \'%s\' to \'%s\'\n", var.c_str(), val.c_str());

		if(var=="track")
			{Trackfile = val;}
		else if(var=="port")
			{networkthread.setPort(val.toInt());}
		else if(var=="remoteplayers")
			{remoteplayers = val.toInt();}
		else
		{
			printf("Unknown variable \'%s\'. Try \'help\'\n", var.c_str());
		}
	}
}

void cmd_show()
{
	printf("Variables:\n");
	printf("  track = %s\n", Trackfile.c_str());
	printf("  port = %d\n", networkthread.getPort());
	printf("  remoteplayers = %d\n", remoteplayers);
	//printf("   = %\n", );
	printf("AI players:\n");
	for(unsigned int i=0; i < AIDescriptions.size(); i++)
		printf("  %s playing with car %s\n",
			AIDescriptions[i].name.c_str(), AIDescriptions[i].carFile.c_str());
}

void cmd_write(const CString &args)
{
	CChatMessage msg;
	msg.m_Message = args;

	CStuntsNet net(0);
	CMessageBuffer buf = msg.getBuffer();
	CIPNumber ipnum;
	ipnum = "localhost";
	buf.setIP(ipnum);
	buf.setPort(1500);
	net.sendData(buf);
}

void executeCommand(const CString &cmd)
{
	if(cmd == "help")
	{
		printf("Commands are:\n"
			"  addai name,carfile   Add an AI player\n"
			"  exit, quit           Terminate the server\n"
			"  write message        Send a message to all players\n"
			"  help                 Display this list\n"
			"  set parameter=value  Set a variable (see below)\n"
			"  show                 Show current variables and players\n"
			"  start                Start the game\n"
			"  stop                 Stop the current game\n"
			"Variables are: \n"
			"  track                Track file\n"
			"  port                 Port where the server should listen\n"
			"  remoteplayers        Maximum number of remote players\n"
			);
	}
	else if(cmd.length() > 6 && cmd.mid(0, 6) == "addai ")
		{cmd_addai(cmd.mid(6));}
	else if(cmd == "exit" || cmd == "quit")
		{cmd_exit();}
	else if(cmd == "start")
		{cmd_start();}
	else if(cmd == "stop")
		{cmd_stop();}
	else if(cmd == "show")
		{cmd_show();}
	else if(cmd.length() > 4 && cmd.mid(0, 4) == "set ")
		{cmd_set(cmd.mid(4));}
	else if(cmd.length() > 6 && cmd.mid(0, 6) == "write ")
		{cmd_write(cmd.mid(6));}
	else
	{
		printf("Unknown command. Try \'help\'\n");
	}
}

int main(int argc, char *argv[])
{
	printf("Welcome to the " PACKAGE " server version " VERSION "\n");

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
			cmd_set(arg);
	}

	printf("\n---Gamecore\n");
	gamecorethread.m_GameCore = new CGameCore;

	printf("Starting network thread\n");
	networkthread.giveClientList(&Clients);
	networkthread.start();
	
	printf("\nNow you can enter commands. Try \'help\' for a list of commands.\n");
	while(true)
	{
		CString cmd = getInput("> ");
		printf("\nYou entered \"%s\"\n", cmd.c_str());
		executeCommand(cmd);
	}

	return EXIT_SUCCESS;
}
