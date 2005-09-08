/***************************************************************************
                          gamecore.cpp  -  The core of the game
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
#include <unistd.h>

#include "clientsim.h"
#include "physics.h"
#include "car.h"

#include "gamecore.h"

CGameCore::CGameCore()
{
	m_World = new CWorld();
	m_PCtrl = NULL;
	m_ClientNet = NULL;
	initLocalGame(""); //just to get it into some state
}

CGameCore::~CGameCore()
{
	unloadGame();

	if(m_ClientNet!=NULL)
		delete m_ClientNet;

	delete m_World;
}

void CGameCore::initLocalGame(const CString &trackfile)
{
	unloadGame();

	//delete client network of previous game sessions
	if(m_ClientNet!=NULL)
		delete m_ClientNet;
	m_ClientNet = NULL;
	stopGame();
	m_TrackFile = trackfile;
}

void CGameCore::initClientGame(const CString &host, unsigned int port)
{
	unloadGame();

	//delete old clientnet only if it's different
	if(m_ClientNet != NULL && !(m_ClientNet->getHost() == host && m_ClientNet->getPort() == port))
	{
		delete m_ClientNet;
		m_ClientNet = NULL;
	}

	//new clientnet if we don't have one
	if(m_ClientNet == NULL)
		{m_ClientNet = new CClientNet(host, port);}

	resetGame();
}

bool CGameCore::addPlayer(CPlayer *p, CObjectChoice choice)
{
	if(m_PCtrl == NULL) return false;

	int id = m_PCtrl->addPlayer(choice);
	p->m_MovingObjectId = id;
	p->m_PlayerId = 0;
	if(id < 0)
		return false; //sim doesn't accept player

	//here the player's name is set
	p->m_Name = choice.m_PlayerName;

	//ad it to the player array
	m_Players.push_back(p);
	return true;
}

void CGameCore::readyAndLoad()
{
	if(m_ClientNet != NULL)
	{
		CClientSim *csim = (CClientSim *)(m_Simulations[0]);
		m_TrackFile = csim->getTrackname();
	}
	
	loadTrackData();
	loadMovObjData();

	if(m_ClientNet != NULL)
	{
		printf("Telling the server that we're ready\n");
		m_ClientNet->sendReady();

		printf("Wait until everybody is ready\n");
		m_ClientNet->wait4Ready();
	}
}

bool CGameCore::update() //true = continue false = leave
{
	if(m_ClientNet != NULL)
	{
		usleep(10000); //< 100 fps. Just to give a server process some CPU time
		m_ClientNet->m_ReceiveBuffer.clear(); //all messages that were not used in the previous turn
		m_ClientNet->receiveData(1); //1 millisecond
	}

	for(unsigned int i=0; i<m_Players.size(); i++)
		m_Players[i]->update(); //Makes moving decisions

	
	bool retval = true;
	for(unsigned int i=0; i<m_Simulations.size(); i++)
	{
		retval = retval && m_Simulations[i]->update(); //Modifies world object
	}

	return retval;
}

void CGameCore::stopGame()
{
	//first make a backup of the hiscore data
	if(theWorld->getNumObjects(CDataObject::eMovingObject) > 0)
		collectHiscoreData();

	//then, unload the game
	resetGame();
}

void CGameCore::resetGame()
{
	unloadGame();

	if(m_ClientNet == NULL) //local game
	{
		m_PCtrl = new CPlayerControl;
		m_Simulations.push_back(new CRuleControl);
		m_Simulations.push_back(new CPhysics(theMainConfig));
	}
	else
	{
		m_PCtrl = new CClientPlayerControl(m_ClientNet);
		m_Simulations.push_back(new CClientSim(this, m_ClientNet));
		//m_Simulations.push_back(new CPhysics(theMainConfig));
		//TODO: CApproximation or CPhysics for slow connections
	}
}

void CGameCore::unloadGame()
{
	for(unsigned int i=0; i<m_Simulations.size(); i++)
		delete m_Simulations[i];
	m_Simulations.clear();

	m_Players.clear();

	if(m_PCtrl!=NULL)
		delete m_PCtrl;
	m_PCtrl = NULL;

	unloadData();
}

void CGameCore::loadTrackData()
{
	printf("---World track data\n");
	m_World->loadObject(m_TrackFile, CParamList(), CDataObject::eTrack);
}

void CGameCore::loadMovObjData()
{
	printf("---World object data\n");
	if(!m_PCtrl->loadObjects()) //loads them into the world
		{printf("Error while loading moving objects\n");}
}

void CGameCore::unloadData()
{
	printf("---World data\n");
	m_World->unloadAll();
}

bool CGameCore::isLocalPlayer(unsigned int ID)
{
	for(unsigned int i=0; i<m_Players.size(); i++)
		if(m_Players[i]->m_MovingObjectId == (int)ID) return true;

	return false;
}

void CGameCore::collectHiscoreData()
{
	m_LastHiscores.clear();

	if(m_ClientNet == NULL) //local game
	{
		for(unsigned int i=0; i < m_Players.size(); i++)
		{
			unsigned int carID = m_Players[i]->m_MovingObjectId;
			CCar *theCar = (CCar *)theWorld->getMovingObject(carID);
			CCarRuleStatus status = theCar->m_RuleStatus;

			float time = status.finishTime - status.startTime + status.penaltyTime;
			if(status.state != CCarRuleStatus::eFinished) time = -1.0;

			SHiscoreEntry e;
			e.name = m_Players[i]->m_Name;
			e.carname = theCar->m_CarName;
			e.time = time;
			e.isNew = true;
			m_LastHiscores.push_back(e);
		}
	}
	else
	{
		//TODO: get hiscore from the server
	}

	//merge with existing hiscore file:
	CHiscoreFile hf(m_TrackFile);
	hf.addEntries(m_LastHiscores);
	m_LastHiscores = hf.getEntries();
}

vector<SHiscoreEntry> CGameCore::getHiscore()
{
	return m_LastHiscores;
}
