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

#include "dataobject.h"
#include "collisionface.h"
#include "material.h"
#include "cstring.h"
#include "cfile.h"

/**
  *@author CJP
  */

class CCollisionModel : public CDataObject {
public: 
	CCollisionModel(CDataManager *manager);
	virtual ~CCollisionModel();

	virtual bool load(const CString &filename, const CParamList &list);

	CString getSubset() const;

	//Bounded volume data:
	float m_BSphere_r; //Bounding sphere
	CVector m_OBB_min, m_OBB_max; //Oriented bounding box

	//The shape itself
	vector<CCollisionFace> m_Faces;

protected:
	enum ePrimitiveType {
		None,
		Triangles,
		Quads,
		Trianglestrip,
		Quadstrip,
		Polygon
	};

	CString m_Subset;

	void determineOBVs();
	void determinePlaneEquations();
};

#endif
