/***************************************************************************
                          body.cpp  -  A rigid body (like a wheel or a car's body)
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

#include <cmath>

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

CVector CBody::getAngularVelocity() const
{
	const dReal *vel = dBodyGetAngularVel(m_ODEBody);
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

CVector CBody::getOrientationVector() const
{
	const dReal *q = dBodyGetQuaternion(m_ODEBody);
	CVector sin_u(q[1], q[2], q[3]);
	float sinus = sin_u.abs();

	float angle = 2 * atan2f(sinus, q[0]);
	
	float mul = 1.0;
	if(sinus > 0.0001) mul = angle / sinus;

	return sin_u * mul;
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

void CBody::setOrientationVector(CVector v)
{
	float angle = v.abs();
	float mul = 1.0;
	if(angle > 0.0001) mul = sin(0.5*angle) / angle;
	CVector sin_u = v * mul;
	dQuaternion q;
	q[0] = cos(0.5*angle);
	q[1] = sin_u.x;
	q[2] = sin_u.y;
	q[3] = sin_u.z;
	dBodySetQuaternion(m_ODEBody, q);
}

void CBody::setVelocity(CVector v)
{
	dBodySetLinearVel(m_ODEBody, v.x, v.y, v.z);
}

void CBody::setAngularVelocity(CVector v)
{
	dBodySetAngularVel(m_ODEBody, v.x, v.y, v.z);
}

void CBody::updateFixedBounds(const CCollisionModel& coll)
{
	m_FBB_min = coll.m_OBB_min;
	m_FBB_max = coll.m_OBB_max;
}
