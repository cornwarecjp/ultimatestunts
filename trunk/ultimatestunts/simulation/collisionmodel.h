/***************************************************************************
                          collisionmodel.h  -  A collision model
                             -------------------
    begin                : wo sep 24 2003
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

#ifndef COLLISIONMODEL_H
#define COLLISIONMODEL_H

#include <vector> //STL vector template
namespace std {}
using namespace std;

#include "material.h"
#include "vector.h"
#include "matrix.h"
#include "cstring.h"
#include "cfile.h"

/**
  *@author CJP
  */

class CCollisionFace : public vector<CVector>
{
public:
	CVector nor;
	float d;
	bool reverse; //points are defined clockwise instead of ccw

	CCollisionFace(){;}
	CCollisionFace(const CCollisionFace &f);
	const CCollisionFace &operator=(const CCollisionFace &f);

	void cull(const CCollisionFace &plane, const CVector &dr);
	const CCollisionFace &operator+=(const CVector &r);
	const CCollisionFace &operator*=(const CMatrix &m);
	const CCollisionFace &operator/=(const CMatrix &m);
};

class CCollisionModel {
public: 
	CCollisionModel();
	virtual ~CCollisionModel();

	virtual bool loadFromFile(CFile *f, CString subset, CMaterial **matarray);

	//Bounded volume data:
	float m_BSphere_r; //Bounding sphere
	CVector m_OBB_min, m_OBB_max; //Oriented bounding box

	//The shape itself
	vector<CCollisionFace> m_Faces;

	CString m_Filename;
	CString m_Subset;

protected:
	enum ePrimitiveType {
		None,
		Triangles,
		Quads,
		Trianglestrip,
		Quadstrip,
		Polygon
	};

	void determineOBVs();
	void determinePlaneEquations();
};

#endif
