/***************************************************************************
                          collisiondetector.h  -  Detects collisions and resting contacts
                             -------------------
    begin                : ma mrt 22 2004
    copyright            : (C) 2004 by CJP
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

#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include <vector> //STL vector template
namespace std {}
using namespace std;

#include "material.h"
#include "vector.h"
#include "collisionmodel.h"
#include "body.h"
/**
  *@author CJP
  */

class CCollisionDetector {
public: 
	CCollisionDetector();
	~CCollisionDetector();

	void calculateCollisions();

protected:
	CVector m_TrackMin, m_TrackMax;
	bool m_FirstUpdate;

	//Object <-> object collisions
	void ObjObjTest(const CBody &body1, const CBody &body2);

	//Object <-> tile collisions
	void ObjTileTest(int nobj, int xtile, int ztile, int htile);
	void tileRotate(CVector &v, int rot);

	//Object <-> track bound collisions
	void calculateTrackBounds();
	void ObjTrackBoundTest(const CBody &body);

	//Generic
	bool sphereTest(const CVector &p1, const CCollisionModel *b1, const CVector &p2, const CCollisionModel *b2);
};

#endif
