/***************************************************************************
                          graphicworld.cpp  -  A graphical version of a world object
                             -------------------
    begin                : do jan 16 2003
    copyright            : (C) 2003 by CJP
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

#include "graphiccar.h"
#include "graphicworld.h"

CGraphicWorld::CGraphicWorld(){
}
CGraphicWorld::~CGraphicWorld(){
}

int CGraphicWorld::addMovingObject(CObjectChoice c)
{
	//future: selecting, using c

	CMovingObject *m = new CGraphicCar; //That's the difference with CWorld

	m_MovObjs.push_back(m);

	int s = m_MovObjs.size();
	printf("Added (graphic) car: total %d moving objects\n", s);

	return s - 1;
}

bool CGraphicWorld::loadFromFile(CString filename)
{
	printf("The world graphics are being loaded from %s\n", filename.c_str());

	return true;
}
