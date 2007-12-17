/***************************************************************************
                          gamecore.h  -  The core of the game
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

#ifndef GAMECORE_H
#define GAMECORE_H

#include <vector>
namespace std {}
using namespace std;

//Frequently used
#include "cstring.h"
#include "lconfig.h"
#include "usmacros.h"
#include "timer.h"

//Simulation stuff
#include "playercontrol.h"
#include "clientplayercontrol.h"
#include "rulecontrol.h"
#include "world.h"

//Player stuff
#include "objectchoice.h"
#include "player.h"

//Other things
#include "filecontrol.h"
#include "hiscorefile.h"

/**
  *@author CJP
  */

class CGameCore {
public: 
	CGameCore();
	virtual ~CGameCore();

	//step 1: initialise network connection etc.
	//the last function call defines the current state
	bool initLocalGame(const CString &trackfile);
	bool initClientGame(const CString &host, unsigned int port, bool keepOldReplay=false);
	bool initReplayGame(const CString &replayfile);

	//step 2: add players
	bool addPlayer(CPlayer *p, CObjectChoice choice);

	//step 3: wait for the start signal and load everything
	typedef bool (* LoadStatusCallback) (const CString &t, float);
	virtual bool readyAndLoad(LoadStatusCallback callBackFun = NULL);

	//step 4: start the game time counter
	void setStartTime(float offset = 0.0);

	//step 5: Play
	virtual bool update(); //true = continue false = leave

	//step 6: Get hiscore+replay data and stop game (go back to step 1 or 2)
	//this does not undo step 1
	//to stop a network connection, init a local game
	void stopGame(bool saveHiscore = true);

	//step 7: Get result information of last game (hiscore, replay)
	CHiscore getHiscore(bool onlyThisGame = false);
	CString getReplayFile() {return m_ReplayFile;}

	//some tools
	//TODO: place these e.g. in CWorld
	bool isLocalPlayer(unsigned int ID);

	float getFPS(){return m_FPS;}

protected:
	CWorld *m_World;
	vector<CPlayer *> m_Players;
	CPlayerControl *m_PCtrl;
	vector<CSimulation *> m_Simulations;
	CClientNet *m_ClientNet;
	CString m_TrackFile;
	
	CString m_ReplayFile;

	enum {eLocalGame, eNetworkGame, eReplayGame} m_GameType;

	CHiscore m_LastHiscores;
	CHiscore m_LastHiscoresThisGame;

	CTimer m_Timer; //for the FPS counter
	float m_TimerOffset; //timer offset relative to game time
	float m_GlobalTimeAccel; //time acceleration (1.0 = normal)
	float m_FPS;

	virtual void loadTrackData();
	virtual void loadMovObjData();

	void collectHiscoreData(bool saveHiscore);

	void resetGame();
	void unloadGame();
	virtual void unloadData();
};

#endif
