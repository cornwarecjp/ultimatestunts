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

#include "gamecorethread.h"
#include "main.h"
#include "movobjinput.h"

CGamecoreThread::CGamecoreThread(){
}
CGamecoreThread::~CGamecoreThread(){
}

void CGamecoreThread::processInput(const CMessageBuffer &b)
{
	m_InputQueue.enter();
	m_InputQueue.push_back(b);
	m_InputQueue.leave();
}

void CGamecoreThread::threadFunc()
{
	//keep running until the game has ended
	//or a stop command is given
	while( m_GameCore->update() && (!m_terminate) )
	{
		processInputQueue();

		//send object data to the clients:
		vector<CDataObject *> objs = theWorld->getObjectArray(CDataObject::eMovingObject);
		for(unsigned int i=0; i < objs.size(); i++)
		{
			CMovingObject *mo = (CMovingObject *)objs[i];
			networkthread.sendToAll(mo->getBuffer());
		}

		//sleep for some time (give some CPU time to e.g. a client)
		usleep(10000); //< 100 fps
	}
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

			if(mo == NULL) continue; //not found

			mo->m_InputData->setBuffer(buffer);
		}
		m_InputQueue.clear();

	}
	m_InputQueue.leave();
}
