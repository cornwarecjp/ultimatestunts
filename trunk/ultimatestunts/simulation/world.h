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
namespace std {}
using namespace std;

#include "tile.h"
#include "bound.h"
#include "movingobject.h"
#include "objectchoice.h"

class CWorld {
public: 
	CWorld();
	virtual ~CWorld();

	vector<CTile> m_Track; //refer to elements from m_TileShapes
	int m_L, m_W, m_H;
	bool loadTrack(CString filename);
	void unloadTrack();

	vector<CMovingObject *> m_MovObjs; //refer to elements from m_MovObjBounds
	bool loadMovObjs(CString filename);
	void unloadMovObjs();

	vector<CShape *>m_TileShapes;
	vector<CBound *>m_MovObjBounds;

	vector<CMaterial *> m_TileMaterials;
	vector<CMaterial *> m_MovObjMaterials;

	vector<CString> m_MovObjSounds;

	CString getBackgroundFilename() const
		{return m_BackgroundFilename;}

protected:
	CMaterial **getMaterialSubset(CString indices);

	CString m_BackgroundFilename;
};

#endif
