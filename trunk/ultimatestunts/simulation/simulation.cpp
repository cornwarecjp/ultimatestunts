/***************************************************************************
                          simulation.cpp  -  description
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

CSimulation::CSimulation(){
}
CSimulation::~CSimulation(){
}

bool CSimulation::addPlayer(CPlayer *p)
{
  int s = m_LocalPlayers.size();
  m_LocalPlayers.resize(s+1);
  m_LocalPlayers[s] = p;

  printf("Added player: total %d players\n", m_LocalPlayers.size());
  return true;
}

bool CSimulation::removePlayer(CPlayer *p)
{
  bool ret = false;
  for(int i=0; i<m_LocalPlayers.size(); i++)
  {
    if(m_LocalPlayers[i]==p)
    {
      ret = true;
      m_LocalPlayers.erase(&(m_LocalPlayers[i]));
      i--;
    }
  }

  if(ret)
    printf("Removed player: %d players left\n", m_LocalPlayers.size());

  return ret;
}
