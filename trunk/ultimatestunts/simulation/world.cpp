/***************************************************************************
                          world.cpp  -  The world: tiles and cars and other objects
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
  for(unsigned int i=0; i<m_MovObjs.size(); i++)
    delete m_MovObjs[i];
}

int CWorld::addMovingObject(CObjectChoice c)
{
	//future: selecting, using c

	CMovingObject *m = new CCar;
	m_MovObjs.push_back(m);

	int s = m_MovObjs.size();
	printf("Added car: total %d moving objects\n", s);

	return s - 1;
}

bool CWorld::loadFromFile(CString filename)
{
	printf("The world is being loaded from %s\n", filename.c_str());

	l=10; w=10; h=2;
	

	return true;
}
