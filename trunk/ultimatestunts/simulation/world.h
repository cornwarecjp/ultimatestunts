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

#include "bound.h"
#include "tilemodel.h"
#include "movingobject.h"
#include "objectchoice.h"
#include "collisiondata.h"

class CTile {
public:
	int m_Model;
	int m_Z, m_R; //height, orientation. 0 <= m_R <= 3

	bool m_isStart, m_isFinish;
	float m_Time;
};

class CWorld {
public: 
	CWorld();
	virtual ~CWorld();

	//Track
	vector<CTile> m_Track; //refer to elements from m_TileModels
	int m_L, m_W, m_H;
	bool loadTrack(CString filename);
	void unloadTrack();

	vector<CTileModel *> m_TileModels;
	vector<CMaterial *> m_TileMaterials;


	//Moving objects
	vector<CMovingObject *> m_MovObjs;
	bool loadMovObjs(CString filename);
	void unloadMovObjs();

	vector<CBound *>m_MovObjBounds;
	vector<CMaterial *> m_MovObjMaterials;
	vector<CString> m_MovObjSounds;

	//Collision data
	CCollisionData *m_CollData;

	//Rule data
	float m_GameStartTime;

	CString getBackgroundFilename() const
		{return m_BackgroundFilename;}

	//debug
	bool printDebug;
protected:
	CMaterial **getMaterialSubset(CString indices);

	CString m_BackgroundFilename;
};

#endif
