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

/**
  *@author CJP
  */

class CGamecoreThread : public CBThread  {
public: 
	CGamecoreThread();
	virtual ~CGamecoreThread();

	virtual void threadFunc();

	void processInput(const CMessageBuffer &b);

	//gamecore data:
	CGameCore *m_GameCore;

protected:

	CCriticalVector<CMessageBuffer> m_InputQueue;
	void processInputQueue();
};

#endif
