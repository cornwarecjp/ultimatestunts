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
#include "bound.h"
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

	void clearData();

	//Object <-> object collisions
	void ObjObjTest(const CBody &body1, const CBody &body2);

	//Object <-> tile collisions
	void ObjTileTest(int nobj, int xtile, int ztile, int htile);
	void tileRotate(CVector &v, int rot);
	void addTileCollision(CBody &body, const CVector &pos, const CVector &nor, float penetr_depth, CMaterial *tileMaterial);

	//Object <-> track bound collisions
	void calculateTrackBounds();
	void ObjTrackBoundTest(const CBody &body);

	//Generic
	bool sphereTest(const CVector &p1, const CCollisionModel *b1, const CVector &p2, const CCollisionModel *b2);
	bool faceTest(const CVector &p1, const CMatrix &o1, const CBound *b1, const CVector &p2, const CMatrix &o2, const CBound *b2, const CCollisionFace &theFace);
};

#endif
