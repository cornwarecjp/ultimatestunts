/***************************************************************************
                          consolethread.cpp  -  The thread that does the stdio
                             -------------------
    begin                : do jan 20 2005
    copyright            : (C) 2005 by CJP
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
#include <unistd.h>

#include "consolethread.h"
#include "main.h"

CConsoleThread::CConsoleThread()
{
	m_Trackfile = "tracks/default.track";
}

CConsoleThread::~CConsoleThread()
{
}

void CConsoleThread::write(const CString &s)
{
	//TODO: maybe do some thread-protection for stdio, if this gives problems
	printf("\n%s\n> ", s.c_str());
	fflush(stdout);
}

void CConsoleThread::startConsole()
{
	printf("\nNow you can enter commands. Try \'help\' for a list of commands.\n");
	while(true)
	{
		CString cmd = getInput("> ");
		printf("\nYou entered \"%s\"\n", cmd.c_str());
		if(!executeCommand(cmd)) break;
	}
}

CString CConsoleThread::getInput(CString question)
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

void CConsoleThread::unReadyPlayers()
{
	Clients.enter();
	for(unsigned int i=0; i < Clients.size(); i++)
		Clients[i].ready = false;
	Clients.leave();
}

void CConsoleThread::wait4ReadyPlayers()
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

void CConsoleThread::clearPlayerLists()
{
	printf("\n---Players\n");
	for(unsigned int i=0; i < m_AIPlayers.size(); i++)
		delete m_AIPlayers[i];
	m_AIPlayers.clear();
	for(unsigned int i=0; i < m_RemotePlayers.size(); i++)
		delete m_RemotePlayers[i];
	m_RemotePlayers.clear();

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

void CConsoleThread::addRemotePlayers()
{
	Clients.enter();

	//add them
	for(unsigned int i=0; i < Clients.playerRequests.size(); i++)
	{
		CString name = CString("Remote player ") + int(i); //TODO

		m_RemotePlayers.push_back(new CPlayer);
		CPlayer *p = m_RemotePlayers.back();

		CObjectChoice &choice = Clients.playerRequests[i];

		if(!gamecorethread.m_GameCore->addPlayer(p, name, choice))
		{
			printf("Sim doesn't accept remote player %s\n", name.c_str());
			delete p;
			m_RemotePlayers.erase(m_RemotePlayers.end());
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

void CConsoleThread::addAIPlayers()
{
	for(unsigned int i=0; i<m_AIDescriptions.size(); i++)
	{
		m_AIPlayers.push_back(new CAIPlayerCar);
		CAIPlayerCar *p = m_AIPlayers.back();

		CString name = m_AIDescriptions[i].name;
		CString car = m_AIDescriptions[i].carFile;

		CObjectChoice choice;
		choice.m_Filename = car;

		if(!gamecorethread.m_GameCore->addPlayer(p, name, choice))
		{
			printf("Sim doesn't accept AI player %s\n", name.c_str());
			delete p;
			m_AIPlayers.erase(m_AIPlayers.end());
			continue;
		}

		//add to the objecechoice list
		ObjectChoices.enter();
		ObjectChoices.push_back(choice);
		ObjectChoices.leave();
	}
}

void CConsoleThread::cmd_start()
{
	//TODO: super-server setup
	gamecorethread.m_GameCore->initLocalGame(m_Trackfile);

	//wait until all players are ready
	wait4ReadyPlayers();
	unReadyPlayers();

	//now send the track info, so that they can load the track
	networkthread.sendToAll("TRACK=" + m_Trackfile);

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

void CConsoleThread::cmd_stop()
{
	printf("Stopping gamecore thread\n");
	if(!gamecorethread.stop())
		printf("For some reason, stopping the thread failed\n");

	clearPlayerLists();
}

void CConsoleThread::cmd_addai(const CString &args)
{
	int pos = args.inStr(',');
	if(pos >= 0)
	{
		CString name = args.mid(0, pos);
		CString car = args.mid(pos+1);
		name.Trim();
		car.Trim();

		printf("Adding AI \'%s\' with car \'%s\'\n", name.c_str(), car.c_str());
		SAIDescr ai;
		ai.name = name;
		ai.carFile = car;
		m_AIDescriptions.push_back(ai);
	}
	else
	{
		printf("Syntax: addai name, carfile\n");
	}
}

void CConsoleThread::cmd_clearai()
{
	printf("Clearing AI list\n");
	m_AIDescriptions.clear();
}

void CConsoleThread::cmd_set(const CString &args)
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
			{m_Trackfile = val;}
		else if(var=="port")
			{networkthread.setPort(val.toInt());}
		else if(var=="maxRequests")
			{
				Clients.enter();
				Clients.maxRequests = val.toInt();
				Clients.leave();
			}
		else
		{
			printf("Unknown variable \'%s\'. Try \'help\'\n", var.c_str());
		}
	}
}

void CConsoleThread::cmd_show()
{
	Clients.enter();
	ObjectChoices.enter();

	printf("Variables:\n");
	printf("  track = %s\n", m_Trackfile.c_str());
	printf("  port = %d\n", networkthread.getPort());
	printf("  maxRequests = %d\n", Clients.maxRequests);

	printf("\nRemote clients:\n");
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
		printf("    Players:\n");
		for(unsigned int j=0; j < c.players.size(); j++)
		{
			int playerID = c.players[j];
			if(playerID < 0)
				{printf("      %d: refused\n", playerID);}
			else
				{printf("      %d: %s\n", playerID,
					ObjectChoices[playerID].m_Filename.c_str());}
		}
	}

	printf("\nAI players:\n");
	for(unsigned int i=0; i < m_AIDescriptions.size(); i++)
		printf("  %s with car %s\n",
			m_AIDescriptions[i].name.c_str(), m_AIDescriptions[i].carFile.c_str());

	printf("\nObjects currently in the game:\n");
	for(unsigned int j=0; j < ObjectChoices.size(); j++)
	{
		printf("  %d: %s\n", j, ObjectChoices[j].m_Filename.c_str());
	}

	ObjectChoices.leave();
	Clients.leave();
}

void CConsoleThread::cmd_write(const CString &args)
{
	CChatMessage msg;
	msg.m_Message = args;

	CMessageBuffer buf = msg.getBuffer();
	networkthread.sendToAll(buf);
}

bool CConsoleThread::executeCommand(const CString &cmd)
{
	if(cmd == "help")
	{
		printf("Commands are:\n"
			"  addai name,carfile   Add an AI player\n"
			"  clearai              Clear the AI list\n"
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
	else if(cmd == "exit" || cmd == "quit")
	{
		cmd_stop();
		return false;
	}
	else if(cmd == "start")
		{cmd_start();}
	else if(cmd == "stop")
		{cmd_stop();}
	else if(cmd == "show")
		{cmd_show();}
	else if(cmd == "clearai")
		{cmd_clearai();}
	else if(cmd.length() > 6 && cmd.mid(0, 6) == "addai ")
		{cmd_addai(cmd.mid(6));}
	else if(cmd.length() > 4 && cmd.mid(0, 4) == "set ")
		{cmd_set(cmd.mid(4));}
	else if(cmd.length() > 6 && cmd.mid(0, 6) == "write ")
		{cmd_write(cmd.mid(6));}
	else
	{
		printf("Unknown command. Try \'help\'\n");
	}

	return true;
}
