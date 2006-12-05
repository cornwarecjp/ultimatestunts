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
#include "collisionface.h"
#include "collisiondata.h"
#include "bound.h"
#include "movingobject.h"
/**
  *@author CJP
  */

class CCollisionDetector {
public: 
	CCollisionDetector();
	~CCollisionDetector();
	void reset();

	vector<CCollisionData> getCollisions(const CMovingObject *obj);

	const CCollisionFace *getGroundFace(const CVector &pos);

	float getLineCollision(const CVector &pos, const CVector &dir);

protected:
	CVector m_TrackMin, m_TrackMax;
	bool m_FirstUpdate;

	//Object <-> object collisions
	CCollisionData ObjObjTest(const CBody &body1, const CBody &body2, CVector vmean);

	//Object <-> tile collisions
	vector<CCollisionData> ObjTileTest(const CMovingObject *theObj, int xtile, int ztile, int htile);
	void tileRotate(CVector &v, int rot);

	//Object <-> track bound collisions
	void calculateTrackBounds();
	vector<CCollisionData> ObjTrackBoundTest(const CBody &body);

	//Ground face tests
	const CCollisionFace *getTileGround(int xtile, int ztile, int htile, const CVector &pos, float &dmax);
	CCollisionFace m_LastGroundFace;

	//Line tile test
	float LineTileTest(const CVector &pos, const CVector &dir, int xtile, int ztile, int htile);

	//Generic
	bool sphereTest(const CVector &p1, const CCollisionModel *b1, const CVector &p2, const CCollisionModel *b2);
	bool faceTest(const CVector &p1, const CMatrix &o1, const CBound *b1, const CVector &p2, const CMatrix &o2, const CBound *b2, const CCollisionFace &theFace);
};

#endif
