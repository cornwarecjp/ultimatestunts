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

#include "gamecore.h"

CGameCore::CGameCore()
{
	m_World = new CWorld();
	m_PCtrl = NULL;
	m_ClientNet = NULL;
}

CGameCore::~CGameCore()
{
	leaveGame(); //just in case we're in one

	delete m_World;
}

void CGameCore::initLocalGame(const CString &trackfile)
{
	leaveGame(); //just in case we're in one

	m_PCtrl = new CPlayerControl;
	m_Simulations.push_back(new CRuleControl(m_World));
	m_Simulations.push_back(new CPhysics(theMainConfig, m_World));
	m_TrackFile = trackfile;
	loadTrackData();
}

void CGameCore::initClientGame(const CString &host, int port)
{
	leaveGame(); //just in case we're in one

	m_ClientNet = new CClientNet;
	m_PCtrl = new CClientPlayerControl(m_ClientNet);
	CClientSim *csim = new CClientSim(m_ClientNet, m_World);
	m_Simulations.push_back(csim);
	m_Simulations.push_back(new CPhysics(theMainConfig, m_World));
	m_TrackFile = csim->getTrackname();
	loadTrackData();
}

bool CGameCore::addPlayer(CPlayer *p, CString name, CObjectChoice choice)
{
	if(m_PCtrl == NULL) return false;

	int id = m_PCtrl->addPlayer(choice);
	p->m_MovingObjectId = id;
	p->m_PlayerId = 0;
	if(id < 0)
		return false; //sim doesn't accept player

	m_Players.push_back(p);
	return true;
}

void CGameCore::startGame()
{
	loadMovObjData();
}

bool CGameCore::update() //true = continue false = leave
{
	for(unsigned int i=0; i<m_Players.size(); i++)
		m_Players[i]->update(); //Makes moving decisions

	
	bool retval = true;
	for(unsigned int i=0; i<m_Simulations.size(); i++)
	{
		retval = retval && m_Simulations[i]->update(); //Modifies world object
	}

	return retval;
}

void CGameCore::leaveGame()
{
	for(unsigned int i=0; i<m_Simulations.size(); i++)
		delete m_Simulations[i];
	m_Simulations.clear();

	m_Players.clear();

	if(m_PCtrl!=NULL)
		delete m_PCtrl;
	m_PCtrl = NULL;

	if(m_ClientNet!=NULL)
		delete m_ClientNet;
	m_ClientNet = NULL;

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
