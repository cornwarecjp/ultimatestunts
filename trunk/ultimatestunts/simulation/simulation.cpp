/***************************************************************************
                          simulation.cpp  -  Base-class for simulations
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

#include <stdio.h>
#include "simulation.h"

CSimulation::CSimulation(CWorld *w)
{
  m_World = w;
}

CSimulation::~CSimulation(){
}

int CSimulation::addPlayer(CObjectChoice choice)
{
	//Default behaviour: always accept a player
	return m_World->addMovingObject(choice);
}

bool CSimulation::removePlayer(int id)
{
	//Should remove car from the world

	return true; //succes

	//Old code:
	/*
  bool ret = false;
  for(unsigned int i=0; i<m_LocalPlayers.size(); i++)
  {
    if(m_LocalPlayers[i]==p)
    {
      ret = true;
//      m_LocalPlayers.erase(&(m_LocalPlayers[i]));    
      m_LocalPlayers.erase(m_LocalPlayers.begin() + i - 1);     // FIXME: TEST IT!
      i--;
    }
  }

  if(ret)
    printf("Removed player: %d players left\n", m_LocalPlayers.size());

  return ret;
	*/

}
