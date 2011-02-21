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

#include "metaserver.h"

#include "consolethread.h"
#include "main.h"

CConsoleThread::CConsoleThread()
{
	m_IsOnline = false;
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
	fflush(stdout);

	CString out;

	while(1)
	{
		char c;
		ssize_t count = read(0, &c, 1);
		if(count <= 0) continue;
		if(c == '\n') break;
		out += c;
	}
	
	/*
	char input[80];
	while(true)
	{
		if(scanf("%s", input) != 1)
		{
			printf("Error reading input\n");
		}

		out += input;
		char c = getchar();
		if(c == '\n') break;
		out += c;
	}
	*/

	return out;
}

void CConsoleThread::cmd_start()
{
	printf("Giving the start command to the gamecore thread\n");
	gamecorethread.GO_startCommand();
}

void CConsoleThread::cmd_stop()
{
	if(!gamecorethread.isRunning())
	{
		printf("Gamecore thread was not running\n");
		return;
	}

	printf("Stopping gamecore thread\n");
	if(!gamecorethread.stop())
		printf("For some reason, stopping the thread failed\n");
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
		CGamecoreThread::SAIDescr ai;
		ai.name = name;
		ai.carFile = car;
		gamecorethread.addAI(ai);
	}
	else
	{
		printf("Syntax: addai name, carfile\n");
	}
}

void CConsoleThread::cmd_clearai()
{
	printf("Clearing AI list\n");
	gamecorethread.clearAI();
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
			{gamecorethread.m_Trackfile = val;}
		else if(var=="port")
		{
			bool wasOnline = m_IsOnline;
			if(wasOnline) cmd_offline();
			networkthread.setPort(val.toInt());
			if(wasOnline) updateMetaServerData();
		}
		else if(var=="serverName")
		{
			networkthread.setServerName(val);
			if(m_IsOnline) updateMetaServerData();
		}
		else if(var=="minPlayers")
			{
				Clients.enter();
				Clients.minPlayers = val.toInt();
				Clients.leave();
			}
		else if(var=="saveHiscore")
			{gamecorethread.m_SaveHiscore = (val == "true");}
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
	printf("        track = %s\n", gamecorethread.m_Trackfile.c_str());
	printf("         port = %d\n", networkthread.getPort());
	printf("   serverName = %s\n", networkthread.getServerName().c_str());
	printf("  minPlayers  = %d\n", Clients.minPlayers);
	printf("  saveHiscore = %s\n", CString(gamecorethread.m_SaveHiscore).c_str());

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

	/*
	printf("\nAI players:\n");
	for(unsigned int i=0; i < gamecorethread.m_AIDescriptions.size(); i++)
		printf("  %s with car %s\n",
			gamecorethread.m_AIDescriptions[i].name.c_str(), gamecorethread.m_AIDescriptions[i].carFile.c_str());
	*/

	printf("\nObjects currently in the game:\n");
	for(unsigned int j=0; j < ObjectChoices.size(); j++)
	{
		printf("  %d: %s\n", j, ObjectChoices[j].m_Filename.c_str());
	}

	ObjectChoices.leave();
	Clients.leave();

	printf("\nGame status: %s\n", gamecorethread.getGameStatus().c_str());

	//A small violation of thread safety
	//we're only reading: shouldn't give too much trouble
	float sfps = gamecorethread.m_GameCore->getFPS();
	float nfps = networkthread.getFPS();
	printf("\nCurrent simulation frame rate: %.1f FPS\n", sfps);
	printf("\nCurrent network    frame rate: %.1f FPS\n", nfps);
}

void CConsoleThread::cmd_online()
{
	updateMetaServerData();
}

void CConsoleThread::cmd_offline()
{
	if(!m_IsOnline) return;
	CMetaServer meta;
	meta.removeServer(networkthread.getPort());
	m_IsOnline = false;
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
			"  online               Register at the metaserver for online gaming\n"
			"  offline              Unregister from the metaserver\n"
			"  start                Start the game\n"
			"  stop                 Stop the current game\n"
			"Variables are: \n"
			"  track                Track file\n"
			"  port                 Network port where the server should listen\n"
			"  serverName           Name for identifying the server\n"
			"  minPlayers           Minumum number of remote players\n"
			"  saveHiscore          Should the server save to the hiscore?\n"
			);
	}
	else if(cmd == "exit" || cmd == "quit")
	{
		cmd_stop();
		cmd_offline();
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
	else if(cmd == "online")
		{cmd_online();}
	else if(cmd == "offline")
		{cmd_offline();}
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

void CConsoleThread::updateMetaServerData()
{
	CMetaServer meta;
	if(meta.setServer(networkthread.getServerName(), networkthread.getPort()))
		m_IsOnline = true;
}

