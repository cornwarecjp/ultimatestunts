/***************************************************************************
                          collisiondata.h  -  description
                             -------------------
    begin                : do aug 19 2004
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

#ifndef COLLISIONDATA_H
#define COLLISIONDATA_H

#include "vector.h"

/**
  *@author CJP
  */

class CCollisionData {
public: 
	float getTangVel();
	float getRadVel();

	CVector pos;
	CVector nor;
	float depth;
	CVector vmean;
	CVector vdiff;
};

#endif
