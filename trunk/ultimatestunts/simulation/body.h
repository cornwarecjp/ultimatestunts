/***************************************************************************
                          body.h  -  A rigid body (like a wheel or a car's body)
                             -------------------
    begin                : di okt 7 2003
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

#ifndef BODY_H
#define BODY_H

#include <vector>
namespace std {}
using namespace std;

#include "collisionmodel.h"
#include "collisiondata.h"
#include "vector.h"
#include "matrix.h"

/**
  *@author CJP
  */


class CBody
{
public:
	CVector m_Position;
	CMatrix m_OrientationMatrix;

	int m_Body;

	//collision properties:
	float m_mu;

	//Other data:
	vector<CCollisionData> m_Collisions; //for usage in the sound subsystem, dust graphics etc.
};

#endif
