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
#include <cstdlib>
#include <unistd.h>

#include <libintl.h>
#define _(String) gettext (String)
#define N_(String1, String2, n) ngettext ((String1), (String2), (n))

#include "usmacros.h"

#include "clientsim.h"
#include "physics.h"
#include "approximation.h"
#include "replayer.h"

#include "car.h"

#include "gamecore.h"

#define TEMPREPLAYFILE "last_race.repl"

CGameCore::CGameCore()
{
	m_World = new CWorld();
	m_PCtrl = NULL;
	m_ClientNet = NULL;
	m_GameType = eLocalGame;
	m_FPS = 0.0;
	initLocalGame(""); //just to get it into some state
}

CGameCore::~CGameCore()
{
	unloadGame();

	if(m_ClientNet!=NULL)
		delete m_ClientNet;

	delete m_World;
}

bool CGameCore::initLocalGame(const CString &trackfile)
{
	unloadGame();

	//delete client network of previous game sessions
	if(m_ClientNet!=NULL)
		delete m_ClientNet;
	m_ClientNet = NULL;

	m_TrackFile = trackfile;
	m_ReplayFile = TEMPREPLAYFILE;
	m_GameType = eLocalGame;

	resetGame();
	return true;
}

bool CGameCore::initClientGame(const CString &host, unsigned int port, bool keepOldReplay)
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

	if(!(m_ClientNet->isConnected()) )
	{
		initLocalGame(m_TrackFile);
		return false;
	}

	m_ReplayFile = TEMPREPLAYFILE;

	//Deleting it will make the client to download it from the server
	//at the end of the game
	if(!keepOldReplay)
		deleteDataFile(TEMPREPLAYFILE);

	m_GameType = eNetworkGame;

	resetGame();
	return true;
}

bool CGameCore::initReplayGame(const CString &replayfile)
{
	unloadGame();

	//delete client network of previous game sessions
	if(m_ClientNet!=NULL)
		delete m_ClientNet;
	m_ClientNet = NULL;
	
	m_ReplayFile = replayfile;
	m_GameType = eReplayGame;

	resetGame();
	return true;
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

bool CGameCore::readyAndLoad(LoadStatusCallback callBackFun)
{
	if(callBackFun != NULL)
		if(!callBackFun(_("Game is being started"), 0.0))
		{
			return false;
		}

	//pre-load:
	switch(m_GameType)
	{
	case eLocalGame:
		break;
	case eNetworkGame:
		{
		CClientSim *csim = (CClientSim *)(m_Simulations[0]); //TODO: less dirty
		m_TrackFile = csim->getTrackname(callBackFun);

		if(m_TrackFile == "") //didn't get track name
		{
			resetGame();
			return false;
		}

		}
		break;
	case eReplayGame:
		{
		CReplayer *replayer = (CReplayer *)(m_Simulations[0]); //TODO: less dirty
		replayer->open(m_ReplayFile, false); //start playing the replay file
		m_TrackFile = replayer->m_TrackFile;

		m_Players.clear();
		m_PCtrl->clearPlayerList();
		for(unsigned int i=0; i < replayer->m_ObjectList.size(); i++)
		{
			CObjectChoice &oc = replayer->m_ObjectList[i];
			printf("Loading player %d: %s, %s\n", i, oc.m_Filename.c_str(), oc.m_PlayerName.c_str());
			if(m_PCtrl->addPlayer(oc) < 0)
			{
				printf("Error: player was refused!\n");
				resetGame();
				return false;
			}
		}

		}
		break;
	default:
		break;
	}

	printf(
	"\n"
	"--------------------\n"
	"Some important data:\n"
	"--------------------\n"
	" Track file: \"%s\"\n"
	"Replay file: \"%s\"\n"
	"--------------------\n",
	m_TrackFile.c_str(),
	m_ReplayFile.c_str()
	);

	if(callBackFun != NULL)
		if(!callBackFun(_("Loading files"), -1.0))
		{
			resetGame();
			return false;
		}

	loadTrackData();
	loadMovObjData();

	if(callBackFun != NULL)
		if(!callBackFun(_("Ready"), -1.0))
		{
			resetGame();
			return false;
		}

	//post-load:
	switch(m_GameType)
	{
	case eLocalGame:
		{
		CReplayer *replayer = (CReplayer *)(m_Simulations[2]); //TODO: less dirty
		replayer->open(m_ReplayFile, true); //start recording the replay file
		}
		break;
	case eNetworkGame:
		{
		printf("Telling the server that we're ready\n");
		m_ClientNet->sendReady();

		printf("Wait until everybody is ready\n");
		m_ClientNet->wait4Ready();
		}
		break;
	case eReplayGame:
	default:
		break;
	}

	return true;
}

void CGameCore::setStartTime(float offset)
{
	//TODO: some time synchronisation between server and client
	theWorld->m_LastTime = -3.01 + offset;
	m_TimerOffset = m_Timer.getTime() - theWorld->m_LastTime;
	m_FPS = 0.0;

	usleep(1000); //< 1000 fps. Just to avoid getting too high FPS in the first frame
}

bool CGameCore::update() //true = continue false = leave
{
	//Game time info:
	float currTime = m_Timer.getTime() - m_TimerOffset;

	float dt = currTime - theWorld->m_LastTime + 0.00001; //to avoid division by zero

	if(dt > 0.5)
	{
#ifdef DEBUGMSG
		printf("Warning: Low update time detected\n");
#endif
		dt = 0.5;
	}

	theWorld->m_Lastdt = dt;
	theWorld->m_LastTime = currTime;

	//FPS:
	float fpsnu = 1.0 / dt;
	m_FPS = 0.9 * m_FPS + 0.1 * fpsnu;

	if(m_GameType == eNetworkGame)
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

	//Do the chatsystem message delivery
	switch(m_GameType)
	{
	case eNetworkGame:
		{
		//send the entire outgoing queue. Receiving is done by CClientSim
		//false = don't wait for the confirmation
		for(unsigned int i=0; i < theWorld->m_ChatSystem.m_OutQueue.size(); i++)
		{
			CMessageBuffer b = theWorld->m_ChatSystem.m_OutQueue[i].getBuffer();
			m_ClientNet->sendDataReliable(b, false);
		}

		theWorld->m_ChatSystem.m_OutQueue.clear();
		}
		break;
	case eLocalGame:
	case eReplayGame:
	default:
		theWorld->m_ChatSystem.loopBack(); //local delivery
		break;
	}

	theWorld->m_ChatSystem.deliverMessages();

	if(!retval && m_GameType == eLocalGame) //if we stop a local game
	{
		//Close the recording of the replay file
		//to make it available as soon as possible for download
		CReplayer *replayer = (CReplayer *)(m_Simulations[2]); //TODO: less dirty
		replayer->close(); //stop recording the replay file
	}

	return retval;
}

void CGameCore::stopGame(bool saveHiscore)
{
	//first make a backup of the hiscore data
	if(theWorld->getNumObjects(CDataObject::eMovingObject) > 0)
		collectHiscoreData(saveHiscore);

	//then, unload the game
	resetGame();
}

void CGameCore::resetGame()
{
	unloadGame();

	switch(m_GameType)
	{
	case eLocalGame:
		{
		m_PCtrl = new CPlayerControl;
		m_Simulations.push_back(new CRuleControl);
		m_Simulations.push_back(new CPhysics(theMainConfig));
		m_Simulations.push_back(new CReplayer(m_PCtrl));
		}
		break;
	case eNetworkGame:
		{
		m_PCtrl = new CClientPlayerControl(m_ClientNet);
		m_Simulations.push_back(new CClientSim(this, m_ClientNet));
		m_Simulations.push_back(new CApproximation);
		//TODO: CPhysics for slow connections
		}
		break;
	case eReplayGame:
		{
		m_PCtrl = new CPlayerControl;
		m_Simulations.push_back(new CReplayer(m_PCtrl));
		m_Simulations.push_back(new CApproximation);
		}
		break;
	default:
		break;
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
	int ret = m_World->loadObject(m_TrackFile, CParamList(), CDataObject::eTrack);

	if(ret < 0)
	{
		printf("Loading the track failed\n");
		exit(EXIT_FAILURE); //TODO: return to menu
	}
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

void CGameCore::collectHiscoreData(bool saveHiscore)
{
	m_LastHiscoresThisGame.clear();

	switch(m_GameType)
	{
	case eLocalGame:
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
			m_LastHiscoresThisGame.push_back(e);
		}

		//Close the replay recorder
		CReplayer *replayer = (CReplayer *)(m_Simulations[2]); //TODO: less dirty
		replayer->close();

		}
		break;
	case eNetworkGame:
		{
		//TODO: special cases for other ways of exiting (e.g. Esc key)

		m_LastHiscoresThisGame = ((CClientSim *)(m_Simulations[0]))->getHiscore();
		}
		break;
	case eReplayGame:
	default:
		//No hiscores to add
		break;
	}

	if(m_GameType != eReplayGame) //in a replay game: don't touch any hiscore data
	{
		//merge with existing hiscore file:
		CHiscoreFile hf(m_TrackFile);
		if(saveHiscore)
			hf.addEntries(m_LastHiscoresThisGame);
		m_LastHiscores = hf.getEntries();
	}

	//Get the replay file (possibly from the server):
	{
		CDataFile f(m_ReplayFile);
	} //~CDataFile destructor called -> closes the file again
	
	if(m_LastHiscores.size() > 0 && m_LastHiscores[0].isNew && m_GameType != eReplayGame)
	{
		//copy it to the track's replay file

		CString destfn = "unrecognised_trackname.repl";
		int dotpos = m_TrackFile.inStr('.');
		if(dotpos >= 0)
			destfn = m_TrackFile.mid(0, dotpos) + ".repl";

		//printf("Copying %s to %s\n", m_ReplayFile.c_str(), destfn.c_str());
		copyDataFile(m_ReplayFile, destfn);
	}
}

CHiscore CGameCore::getHiscore(bool onlyThisGame)
{
	if(onlyThisGame)
		return m_LastHiscoresThisGame;

	return m_LastHiscores;
}
