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

CSimulation::CSimulation(CWorld *w, bool leading)
{
  m_World = w;
  m_Leading = leading;
}

CSimulation::~CSimulation(){
}

bool CSimulation::addPlayer(CPlayer *p, CObjectChoice choice)
{
  int s = m_LocalPlayers.size();
  m_LocalPlayers.resize(s+1);
  m_LocalPlayers[s] = p;

  printf("Added player: total %d players\n", m_LocalPlayers.size());

  if(m_Leading)
  {
    printf("Giving the player a car\n");
    p->m_MovingObjectId = m_World->addMovingObject(choice);
    printf("Car ID: %d\n", p->m_MovingObjectId);

    p->m_PlayerId = s;
    printf("Player ID: %d\n", p->m_PlayerId);
  }

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

void CSimulation::Update()
{} //Base class does nothing
