/***************************************************************************
                          world.cpp  -  description
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

#include "world.h"
#include "car.h"

CWorld::CWorld(){
}
CWorld::~CWorld(){
  for(int i=0; i<m_MovObjs.size(); i++)
    delete m_MovObjs[i];
}

int CWorld::addMovingObject(CObjectChoice c)
{
  //future: selecting, using c

  CMovingObject *m = new CCar;

  int s = m_MovObjs.size();
  m_MovObjs.resize(s+1);
  m_MovObjs[s] = m;

  printf("Added car: total %d moving objects\n", m_MovObjs.size());

  return s;
}
