/***************************************************************************
                          collision.h  -  The data describing a collision
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
#ifndef COLLISION_H
#define COLLISION_H

class CCollision {
public:
	float t; //0 <= t <= 1

	//position in world coordinates
	//normal pointing from object1.body1 to object2.body2
	CVector pos, nor;

	float p; //impulse transfer parallel to normal for non-resting contact (no friction included)
	//float penetrationDepth;
	bool isResting;
	

	//objects & materials. 1 = the object itself 2 = the other object
	int object1, object2; //the object that collided
	int body1, body2; //the bodies that collided
	int mat1, mat2; //The two materials
};

#endif
