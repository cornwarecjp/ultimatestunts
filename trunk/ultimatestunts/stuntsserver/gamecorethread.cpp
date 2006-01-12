/***************************************************************************
                          gamecorethread.cpp  -  The thread of the simulation
                             -------------------
    begin                : wo jan 12 2005
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

#include <unistd.h>
#include <cstdio>

#include <libintl.h>
#define _(String) gettext (String)
#define N_(String1, String2, n) ngettext ((String1), (String2), (n))

#include "gamecorethread.h"
#include "main.h"
#include "movobjinput.h"
#include "timer.h"
#include "netmessages.h"

CGamecoreThread::CGamecoreThread()
{
	m_SaveHiscore = true;
	m_Trackfile = "tracks/default.track";

	m_GO_minPlayers = false;
	m_GO_startCommand = false;
	m_GameStatus = "Error: uninitialised status"; //should never occur
}

CGamecoreThread::~CGamecoreThread(){
}

void CGamecoreThread::GO_minPlayers()
{
	consolethread.write("Minimum number of players reached");
	m_GO_minPlayers = true;

	if(m_GO_minPlayers && m_GO_startCommand)
		start(); //start the thread
}

void CGamecoreThread::GO_startCommand()
{
	consolethread.write("Start command given");
	m_GO_startCommand = true;

	if(m_GO_minPlayers && m_GO_startCommand)
		start(); //start the thread
}

CString CGamecoreThread::getGameStatus()
{
	if(isRunning())
	{
		return m_GameStatus; //loading, waiting, running etc.
	}
	else if(m_GO_minPlayers)
	{
		return _("Waiting for the server to start");
	}
	else
	{
		Clients.enter();
		unsigned int players = Clients.playerRequests.size() + Clients.AIrequests;
		unsigned int minimum = Clients.minPlayers;
		Clients.leave();

		CString status;
		status.format(
			N_("Waiting for %d remaining player to join", "Waiting for %d remaining players to join",
			minimum - players), 80, minimum - players);
		return status;
	}
}

void CGamecoreThread::processInput(const CMessageBuffer &b)
{
	m_InputQueue.enter();
	m_InputQueue.push_back(b);
	m_InputQueue.leave();
}

void CGamecoreThread::threadFunc()
{
	startGame();

	m_GameStatus = _("Running a game");

	float t = CTimer().getTime();

	//keep running until the game has ended
	//or a stop command is given
	while( m_GameCore->update() && (!m_terminate) )
	{
		processInputQueue();

		//send object data to the clients:
		float tnow = CTimer().getTime();
		if(tnow - t > 0.04) //25 fps network updates
		{
			t = tnow;

			vector<CDataObject *> objs = theWorld->getObjectArray(CDataObject::eMovingObject);
			for(unsigned int i=0; i < objs.size(); i++)
			{
				//send object state
				CMovingObject *mo = (CMovingObject *)objs[i];
				networkthread.sendToAll(mo->getBuffer());

				//send messages
				for(unsigned int j=0; j < mo->m_IncomingMessages.size(); j++)
				{
					CChatMessage &msg = mo->m_IncomingMessages[j];
					msg.m_ToMovingObject = mo->getMovObjID(); //prevents multiplication of "to all" messages
					CMessageBuffer b = msg.getBuffer();
					b.setAC(1); //reliable
					networkthread.sendToAll(b);
				}
				mo->m_IncomingMessages.clear();
			}
		}

		//sleep for some time (give some CPU time to e.g. a client)
		usleep(10000); //< 100 fps
	}

	m_GameStatus = _("Stopping the game");

	m_GameCore->stopGame(m_SaveHiscore);

	//now the game has ended, send the hiscore
	CMessageBuffer b = m_GameCore->getHiscore(true).getBuffer(); //true: only hiscore entries of this game
	b.setAC(1);
	networkthread.sendToAll(b);

	clearPlayerLists();
	m_GO_minPlayers = false;
	m_GO_startCommand = false;
}

void CGamecoreThread::processInputQueue()
{
	m_InputQueue.enter();
	if(m_InputQueue.size() > 0)
	{
		//the moving objects:
		vector<CDataObject *> objs = theWorld->getObjectArray(CDataObject::eMovingObject);

		for(unsigned int i=0; i < m_InputQueue.size(); i++)
		{
			CMessageBuffer &buffer = m_InputQueue[i];

			CBinBuffer b = buffer.getData();
			unsigned int pos=0;
			Uint8 ID = b.getUint8(pos);

			
			//find the corresponding moving object
			CMovingObject *mo = NULL;
			//first candidate
			if(ID < objs.size())
			{
				CMovingObject *tmp = (CMovingObject *)objs[ID];
				if(tmp->getMovObjID() == ID)
					mo = tmp;
			}
			//all other objects
			if(mo == NULL)
				for(unsigned int j=0; j < objs.size(); j++)
				{
					CMovingObject *tmp = (CMovingObject *)objs[j];
					if(tmp->getMovObjID() == ID)
						mo = tmp;
				}

			if(mo == NULL)
			{
				consolethread.write("Unidentified Input Message (UIM)");
				continue; //not found
			}

			mo->m_InputData->setBuffer(buffer);

			//consolethread.write(CString("Steering input ") + ID + ":" + mo->m_InputData->m_Right);
		}
		m_InputQueue.clear();

	}
	m_InputQueue.leave();
}

void CGamecoreThread::unReadyPlayers()
{
	Clients.enter();
	for(unsigned int i=0; i < Clients.size(); i++)
		Clients[i].ready = false;
	Clients.leave();
}

void CGamecoreThread::wait4ReadyPlayers()
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

void CGamecoreThread::clearPlayerLists()
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
	Clients.AIrequests = 0;
	Clients.leave();

	ObjectChoices.enter();
	ObjectChoices.clear();
	ObjectChoices.leave();
}

void CGamecoreThread::addRemotePlayers()
{
	Clients.enter();

	//add them
	for(unsigned int i=0; i < Clients.playerRequests.size(); i++)
	{
		m_RemotePlayers.push_back(new CPlayer);
		CPlayer *p = m_RemotePlayers.back();

		CObjectChoice &choice = Clients.playerRequests[i];

		if(!gamecorethread.m_GameCore->addPlayer(p, choice))
		{
			printf("Sim doesn't accept remote player %s\n", choice.m_PlayerName.c_str());
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

void CGamecoreThread::addAI(SAIDescr ai)
{
	m_AIDescriptions.push_back(ai);
	Clients.enter();
	Clients.AIrequests++;
	Clients.leave();

	if(Clients.reachedMinimum_safe()) GO_minPlayers();
}

void CGamecoreThread::clearAI()
{
	m_AIDescriptions.clear();
	Clients.enter();
	Clients.AIrequests = 0;
	Clients.leave();
}

void CGamecoreThread::addAIPlayers()
{
	for(unsigned int i=0; i<m_AIDescriptions.size(); i++)
	{
		m_AIPlayers.push_back(new CAIPlayerCar);
		CAIPlayerCar *p = m_AIPlayers.back();

		CString name = m_AIDescriptions[i].name;
		CString car = m_AIDescriptions[i].carFile;

		CObjectChoice choice;
		choice.m_Filename = car;
		choice.m_PlayerName = name;

		if(!gamecorethread.m_GameCore->addPlayer(p, choice))
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

void CGamecoreThread::startGame()
{
	consolethread.write(_("Game is being started"));

	m_GameStatus = _("Game is being started");

	//TODO: super-server setup
	m_GameCore->initLocalGame(m_Trackfile);

	//wait until all players are ready
	m_GameStatus = _("Waiting for client processes");
	wait4ReadyPlayers();
	unReadyPlayers();

	//now send the track info to clients, so that they can load the track
	networkthread.sendToAll(USNET_TRACK + m_Trackfile);

	consolethread.write(_("Loading"));
	clearPlayerLists();
	addRemotePlayers(); //add remote human and AI players
	addAIPlayers(); //add local AI players

	//load the track and moving objects
	m_GameCore->readyAndLoad();

	//wait a second time for a ready signal
	m_GameStatus = _("Waiting for client processes");
	wait4ReadyPlayers();
	unReadyPlayers();

	//Then tell everybody to start the game, and start it here at the same time
	networkthread.sendToAll(USNET_READY);
	m_GameCore->setStartTime();
}
