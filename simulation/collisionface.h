/***************************************************************************
                          collisionface.h  -  description
                             -------------------
    begin                : wo nov 26 2003
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

#ifndef COLLISIONFACE_H
#define COLLISIONFACE_H

#include <vector> //STL vector template
namespace std {}
using namespace std;

#include "vector.h"
#include "matrix.h"
#include "material.h"

/**
  *@author CJP
  */

class CCollisionFace : public vector<CVector>  {
public: 
	CVector nor;
	float d;
	bool reverse; //points are defined clockwise instead of ccw

	bool isWater;
	bool isSurface;

	CMaterial *material; //points to material array

	CCollisionFace();
	CCollisionFace(const CCollisionFace &f);
	const CCollisionFace &operator=(const CCollisionFace &f);

	void cull(const CCollisionFace &plane, const CVector &dr);
	const CCollisionFace &operator+=(const CVector &r);
	const CCollisionFace &operator*=(const CMatrix &m);
	const CCollisionFace &operator/=(const CMatrix &m);

	bool isInside(const CVector &pt) const;
};

#endif
