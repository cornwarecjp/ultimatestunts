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
#include "lconfig.h"

class CWorld {
public: 
	CWorld(const CLConfig &conf);
	virtual ~CWorld();

	vector<CTile> m_Track; //refer to elements from m_TileShapes
	int m_L, m_W, m_H;
	bool loadTrack(CString filename);
	void unloadTrack();

	vector<CMovingObject *> m_MovObjs; //refer to elements from m_MovObjBounds
	bool loadMovObjs(CString filename);
	void unloadMovObjs();

protected:
	virtual CShape *createShape();
	virtual CBound *createBound();
	virtual CMaterial *createMaterial();

	vector<CShape *>m_TileShapes;
	vector<CBound *>m_MovObjBounds;

	vector<CMaterial *> m_TileMaterials;
	vector<CMaterial *> m_MovObjMaterials;

	CMaterial **getMaterialSubset(CString indices);

	CString m_DataDir;
	int m_TexMaxSize;
};

#endif
