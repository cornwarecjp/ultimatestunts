/***************************************************************************
                          simulation.h  -  Base-class for simulations
                             -------------------
    begin                : Wed Dec 4 2002
    copyright            : (C) 2002 by CJP
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

#ifndef SIMULATION_H
#define SIMULATION_H


/**
  *@author CJP
  */

#include <vector> //STL vector template
namespace std {}
using namespace std;

#include "player.h"
#include "world.h"
#include "objectchoice.h"

#include "movingobject.h"

class CSimulation {
public: 
	CSimulation(CWorld *w);
	virtual ~CSimulation();

	//player init:
	virtual int addPlayer(CObjectChoice choice);
	virtual bool loadObjects();

	//while playing:
	virtual bool update() =0;

protected:

  //Services of the base-class; derived classes don't have to use these
	vector<CObjectChoice> m_LocalChoices;
	CWorld *m_World;
};

#endif
