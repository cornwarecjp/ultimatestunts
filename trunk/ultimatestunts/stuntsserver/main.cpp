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

#include "lconfig.h"
#include "filecontrol.h"

#include "main.h"

#include "aiplayercar.h"
#include "remoteplayer.h"

#include "objectchoice.h"
#include "chatmessage.h"

#include "client.h"
#include "gamecorethread.h"
#include "networkthread.h"

//The game objects
CCriticalVector<CObjectChoice> ObjectChoices;
CGamecoreThread gamecorethread;
CNetworkThread networkthread;
CClientList Clients;

vector<CRemotePlayer *> RemotePlayers;
vector<CAIPlayerCar *> AIPlayers;

//The variables
CString Trackfile = "tracks/default.track";

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

void unReadyPlayers()
{
	Clients.enter();
	for(unsigned int i=0; i < Clients.size(); i++)
		Clients[i].ready = false;
	Clients.leave();
}

void wait4ReadyPlayers()
{
	printf("waiting until all clients are ready\n");
	while(true)
	{
		bool ready = true;

		Clients.enter();
		for(unsigned int i=0; i < Clients.size(); i++)
			if(!Clients[i].ready)
				{ready = false; break;}
		Clients.leave();

		if(ready) break;

		sleep(1);
	}
	printf("OK, they are ready\n");
}

void clearPlayerLists()
{
	printf("\n---Players\n");
	for(unsigned int i=0; i < AIPlayers.size(); i++)
		delete AIPlayers[i];
	AIPlayers.clear();
	for(unsigned int i=0; i < RemotePlayers.size(); i++)
		delete RemotePlayers[i];
	RemotePlayers.clear();

	Clients.enter();
	for(unsigned int i=0; i < Clients.size(); i++)
	{
		Clients[i].players.clear();
		Clients[i].ready = false;
	}
	Clients.leave();

	ObjectChoices.enter();
	ObjectChoices.clear();
	ObjectChoices.leave();
}

void addRemotePlayers()
{
	Clients.enter();

	//add them
	for(unsigned int i=0; i < Clients.playerRequests.size(); i++)
	{
		CString name = CString("Remote player ") + int(i); //TODO

		RemotePlayers.push_back(new CRemotePlayer);
		CRemotePlayer *p = RemotePlayers.back();

		CObjectChoice &choice = Clients.playerRequests[i];

		if(!gamecorethread.m_GameCore->addPlayer(p, name, choice))
		{
			printf("Sim doesn't accept remote player %s\n", name.c_str());
			delete p;
			RemotePlayers.erase(RemotePlayers.end());
			continue;
		}

		//add to the objecechoice list
		ObjectChoices.enter();
		ObjectChoices.push_back(choice);
		ObjectChoices.leave();
	}

	//remove player requests:
	Clients.playerRequests.clear();

	//Make references in the client info
	for(unsigned int i=0; i < Clients.size(); i++)
	{
		Clients[i].players = Clients[i].playerRequests;
		Clients[i].playerRequests.clear();
		Clients[i].ready = false;
	}

	Clients.leave();
}

void addAIPlayers()
{
	for(unsigned int i=0; i<AIDescriptions.size(); i++)
	{
		AIPlayers.push_back(new CAIPlayerCar);
		CAIPlayerCar *p = AIPlayers.back();

		CString name = AIDescriptions[i].name;
		CString car = AIDescriptions[i].carFile;

		CObjectChoice choice;
		choice.m_Filename = car;

		if(!gamecorethread.m_GameCore->addPlayer(p, name, choice))
		{
			printf("Sim doesn't accept AI player %s\n", name.c_str());
			delete p;
			AIPlayers.erase(AIPlayers.end());
			continue;
		}

		//add to the objecechoice list
		ObjectChoices.enter();
		ObjectChoices.push_back(choice);
		ObjectChoices.leave();
	}
}

void cmd_start()
{
	//TODO: super-server setup
	gamecorethread.m_GameCore->initLocalGame(Trackfile);

	//wait until all players are ready
	wait4ReadyPlayers();
	unReadyPlayers();

	//now send the track info, so that they can load the track
	networkthread.sendToAll("TRACK=" + Trackfile);

	clearPlayerLists();
	addRemotePlayers(); //add remote human and AI players
	addAIPlayers(); //add local AI players

	//load the track and moving objects
	gamecorethread.m_GameCore->readyAndLoad();

	//wait a second time for a ready signal
	wait4ReadyPlayers();
	unReadyPlayers();

	//Then tell everybody to start the game
	networkthread.sendToAll("READY");

	printf("Starting gamecore thread\n");
	if(!gamecorethread.start())
		printf("For some reason, starting the thread failed\n");
}

void cmd_stop()
{
	printf("Stopping gamecore thread\n");
	if(!gamecorethread.stop())
		printf("For some reason, stopping the thread failed\n");

	clearPlayerLists();
}

void cmd_exit()
{
	cmd_stop(); //to make sure that nothing is running

	printf("\n---Gamecore\n");
	delete gamecorethread.m_GameCore;
	
	printf("Stopping network thread\n");
	if(!networkthread.stop())
		printf("For some reason, stopping the thread failed\n");

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
		else if(var=="maxRequests")
			{Clients.maxRequests = val.toInt();}
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
	printf("  maxRequests = %d\n", Clients.maxRequests);
	//printf("   = %\n", );
	printf("\nRemote players:\n");
	Clients.enter();
	for(unsigned int i=0; i < Clients.size(); i++)
	{
		CClient &c = Clients[i];
		printf("  Client %d: %s port %d\n", i, c.ip.toString().c_str(), c.port);
		printf("    Requests:\n");
		for(unsigned int j=0; j < c.playerRequests.size(); j++)
		{
			int reqID = c.playerRequests[j];
			if(reqID < 0)
				{printf("      %d: refused\n", reqID);}
			else
				{printf("      %d: %s\n", reqID, Clients.playerRequests[reqID].m_Filename.c_str());}
		}
	}
	Clients.leave();

	printf("\nAI players:\n");
	for(unsigned int i=0; i < AIDescriptions.size(); i++)
		printf("  %s playing with car %s\n",
			AIDescriptions[i].name.c_str(), AIDescriptions[i].carFile.c_str());

	printf("Players:\n");
	ObjectChoices.enter();
	for(unsigned int j=0; j < ObjectChoices.size(); j++)
	{
		printf("  %s\n", ObjectChoices[j].m_Filename.c_str());
	}
	ObjectChoices.leave();
}

void cmd_write(const CString &args)
{
	CChatMessage msg;
	msg.m_Message = args;

	CMessageBuffer buf = msg.getBuffer();
	networkthread.sendToAll(buf);
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
			"  maxRequests          Maximum number of remote players\n"
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
