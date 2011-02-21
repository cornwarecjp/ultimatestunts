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

#include <cmath>
#include "pi.h"

CTECamera::CTECamera()
{
	m_XAngle = -3.5;
	m_YAngle = 0.3;
	m_Distance = 10.0*TILESIZE;
	CVector m_TargetPos = CVector(m_Distance, 0, m_Distance);

	updatePosition();
}

CTECamera::~CTECamera(){
}

void CTECamera::setTargetPos(CVector p)
{
	m_TargetPos = p;
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

	if(m_YAngle < 0.0) m_YAngle = 0.0;
	if(m_YAngle > 0.5*M_PI) m_YAngle = 0.5*M_PI;

	updatePosition();
}

void CTECamera::zoomOut(float factor)
{
	m_Distance *= factor;

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
	m_Position = CVector(0.0, 0.0, m_Distance);
	m_Position *= m_Orientation;
	m_Position += m_TargetPos;
}
