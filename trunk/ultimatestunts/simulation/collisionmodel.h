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
	CVector nor;
	float d;
};

class CCollisionModel {
public: 
	CCollisionModel();
	virtual ~CCollisionModel();

	virtual bool loadFromFile(CFile *f, CString subset, CMaterial **matarray);

	//Bounded volume data:
	float m_BSphere_r; //Bounding sphere
	CVector m_FBB_min, m_FBB_max; //Fixed bounding box
	CVector m_OBB_min, m_OBB_max; //Oriented bounding box

	//The shape itself
	vector<CCollisionFace> m_Faces;

	virtual void calculateFixedVolumes(const CMatrix &ori);

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
};

#endif
