/***************************************************************************
                          body.cpp  -  description
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

#include "body.h"
#include "world.h"

void CBody::createODE()
{
	m_ODEBody = dBodyCreate(theWorld->m_ODEWorld);
}

void CBody::destroyODE()
{
	dBodyDestroy(m_ODEBody);
}

CVector CBody::getPosition() const
{
	const dReal *pos = dBodyGetPosition(m_ODEBody);
	CVector ret;
	ret.x = pos[0];
	ret.y = pos[1];
	ret.z = pos[2];
	return ret;
}

CVector CBody::getVelocity() const
{
	const dReal *vel = dBodyGetLinearVel(m_ODEBody);
	CVector ret;
	ret.x = vel[0];
	ret.y = vel[1];
	ret.z = vel[2];
	return ret;
}

CMatrix CBody::getOrientationMatrix() const
{
	CMatrix ret;
	
	const dReal *rot = dBodyGetRotation(m_ODEBody);

	for(unsigned int i=0; i<3; i++)
		for(unsigned int j=0; j<3; j++)
			ret.setElement(j, i, rot[4*i+j]);

	return ret;
}

void CBody::setPosition(CVector v)
{
	dBodySetPosition(m_ODEBody, v.x, v.y, v.z);
}

void CBody::setOrientationMatrix(const CMatrix &m)
{
	dMatrix3 R;
	for(unsigned int i=0; i<3; i++)
		for(unsigned int j=0; j<3; j++)
			R[4*i+j] = m.Element(j, i);

	for(unsigned int j=0; j<3; j++)
		R[j+3] = 0.0;

	dBodySetRotation(m_ODEBody, R);
}

void CBody::updateFixedBounds(const CCollisionModel& coll)
{
	m_FBB_min = coll.m_OBB_min;
	m_FBB_max = coll.m_OBB_max;
}
