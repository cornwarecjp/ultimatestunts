/***************************************************************************
                          body.h  -  description
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

#include "collisionmodel.h"
#include "vector.h"
#include "matrix.h"

/**
  *@author CJP
  */

class CWorld;

class CBody
{
public:
	int m_Body;
	CVector m_Position;
	CMatrix m_OrientationMatrix;
	CVector m_PreviousPosition;
	CMatrix m_PreviousOrientationMatrix;

	CVector m_FBB_min, m_FBB_max; //Fixed bounding box

	void updateFixedBounds(const CCollisionModel& coll);
};

#endif
