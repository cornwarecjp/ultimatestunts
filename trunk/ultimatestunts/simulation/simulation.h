/***************************************************************************
                          simulation.h  -  description
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
#include "player.h"
#include "world.h"
#include "objectchoice.h"

#include "movingobject.h"

class CSimulation {
public: 
	CSimulation(CWorld *w, bool leading=true);
	virtual ~CSimulation();

  virtual bool addPlayer(CPlayer *p, CObjectChoice choice);
  virtual bool removePlayer(CPlayer *p);

  virtual void Update();

protected:

  //Services of the base-class; derived classes don't have to use these
  vector<CPlayer *> m_LocalPlayers;
  CWorld *m_World;
  bool m_Leading;
};

#endif
