/***************************************************************************
                          bound.h  -  Plane-based collision model
                             -------------------
    begin                : vr jan 24 2003
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

#ifndef BOUND_H
#define BOUND_H

#include <vector> //STL vector template
namespace std {}
using namespace std;

#include "material.h"
#include "vector.h"
#include "cstring.h"

/**
  *@author CJP
  */

class CBound {
public: 
	CBound();
	~CBound();

	bool loadFromFile(CString filename, CMaterial **matarray);

	float m_BSphere_r;
	CVector m_BBox_min, m_BBox_max;

	vector<CVector> m_Planes;

	CString m_Filename;
};

#endif
