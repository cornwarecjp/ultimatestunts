/***************************************************************************
                          world.h  -  The world: tiles and cars and other objects
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

#ifndef WORLD_H
#define WORLD_H


/**
  *@author CJP
  */
#include <vector> //STL vector template

#include "tile.h"
#include "movingobject.h"
#include "objectchoice.h"

class CWorld {
public: 
	CWorld();
	virtual ~CWorld();

	vector<CTile *> m_Track; //elements from m_Tiles?
	vector<int> m_Rotation; //Rotation of tiles
	vector<int> m_Height; //Height of tiles
	int l, w, h;
	virtual bool loadFromFile(CString filename);

	vector<CMovingObject *> m_MovObjs; //elements from m_Objects?
	virtual int addMovingObject(CObjectChoice c);

protected:
	//Track + tiles:
	//vector<CTile *> m_Tiles;

	//Cars + other objects:
	//vector<CMovingObject *> m_Objects;
};

#endif
