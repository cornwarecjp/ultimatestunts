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

#include <ode/ode.h>
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

struct SCollisionData
{
};

class CBody
{
public:
	void createODE();
	void destroyODE();

	void updateFixedBounds(const CCollisionModel& coll);

	CVector getPosition() const;
	CMatrix getOrientationMatrix() const;
	CVector getOrientationVector() const;
	CVector getVelocity() const;
	CVector getAngularVelocity() const;

	void setPosition(CVector v);
	void setOrientationMatrix(const CMatrix &m);
	void setOrientationVector(CVector v);
	void setVelocity(CVector v);
	void setAngularVelocity(CVector v);

	int m_Body;
	CVector m_FBB_min, m_FBB_max; //Fixed bounding box

	
	//ODE data
	dBodyID m_ODEBody;

	//collision properties:
	float m_mu;

	//Other data:
	vector<CCollisionData> m_Collisions; //for usage in the sound subsystem, dust graphics etc.
};

#endif
