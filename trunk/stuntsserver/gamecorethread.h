/***************************************************************************
                          gamecorethread.h  -  The thread of the simulation
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

#ifndef GAMECORETHREAD_H
#define GAMECORETHREAD_H

#include "bthread.h"
#include "gamecore.h"
#include "criticalvector.h"
#include "aiplayercar.h"

/**
  *@author CJP
  */

class CGamecoreThread : public CBThread  {
public: 
	CGamecoreThread();
	virtual ~CGamecoreThread();

	//These functions all need to be called once, then the game starts
	//Please don't call start() directly
	void GO_minPlayers();
	void GO_startCommand();

	//Loading status
	CString getGameStatus();

	void processInput(const CMessageBuffer &b); //put b on the input queue

	virtual void threadFunc();

	//gamecore data:
	CGameCore *m_GameCore;

	//input
	bool m_SaveHiscore;
	CString m_Trackfile;

	struct SAIDescr
	{
		CString name;
		CString carFile;
	};
	void addAI(SAIDescr ai);
	void clearAI();

protected:
	bool m_GO_minPlayers, m_GO_startCommand;

	//Functions for the loading procedure:
	void unReadyPlayers();
	void wait4ReadyPlayers();
	void startGame();
	CString m_GameStatus; //used when thread is running

	CCriticalVector<CMessageBuffer> m_InputQueue;
	void processInputQueue();

	void clearPlayerLists();
	void addRemotePlayers();
	void addAIPlayers();
	vector<CPlayer *> m_RemotePlayers;
	vector<CAIPlayerCar *> m_AIPlayers;

	vector<SAIDescr> m_AIDescriptions;
};

#endif
