/***************************************************************************
                          tecamera.cpp  -  The camera of the track editor
                             -------------------
    begin                : ma mei 23 2005
    copyright            : (C) 2005 by CJP
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

#include "tecamera.h"
#include "usmacros.h"

CTECamera::CTECamera()
{
	m_XAngle = m_YAngle = 0.0;
	CVector m_TargetPos = TILESIZE * CVector(10,0,10);

	updatePosition();
}

CTECamera::~CTECamera(){
}

void CTECamera::moveForward(float dist)
{
	CVector dx = m_Orientation * CVector(0,0,-dist);
	m_TargetPos += (dx - dx.component(CVector(0,1,0)));
	updatePosition();
}
void CTECamera::turnRight(float angle)
{
	m_XAngle -= angle;
	updatePosition();
}
void CTECamera::turnUp(float angle)
{
	m_YAngle += angle;
	updatePosition();
}

void CTECamera::updatePosition()
{
	//Orientation
	CMatrix vertmat, hormat;
	vertmat.setRotation(CVector(m_YAngle, 0.0, 0.0));
	hormat.setRotation(CVector(0.0, m_XAngle, 0.0));
	m_Orientation = vertmat * hormat;

	//Distance
	m_Position = CVector(0.0, 0.0, 3.0*TILESIZE);
	m_Position *= m_Orientation;
	m_Position += m_TargetPos;
}
