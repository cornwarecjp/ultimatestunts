/***************************************************************************
                          editcamera.cpp  -  description
                             -------------------
    begin                : wo apr 9 2003
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

#include "editcamera.h"
#include "usmacros.h"

CEditCamera::CEditCamera()
{
	m_XAngle = m_YAngle = 0.0;
	m_Dist = 2 * TILESIZE;

	m_TileCenter = false;

	updatePosition();
}

CEditCamera::~CEditCamera(){
}

void CEditCamera::setDist(float dist)
{m_Dist = dist; updatePosition();}
void CEditCamera::setXAngle(float xangle)
{m_XAngle = xangle; updatePosition();}
void CEditCamera::setYAngle(float yangle)
{m_YAngle = yangle; updatePosition();}

void CEditCamera::incrDist(float dist)
{m_Dist += dist; updatePosition();}
void CEditCamera::incrXAngle(float xangle)
{m_XAngle += xangle; updatePosition();}
void CEditCamera::incrYAngle(float yangle)
{m_YAngle += yangle; updatePosition();}

void CEditCamera::flipCameraCenter()
{m_TileCenter = !m_TileCenter; updatePosition();}

void CEditCamera::updatePosition()
{
	//Distance
	m_Position = CVector(0.0, 0.0, m_Dist);

	//Orientation
	m_Orientation = CMatrix(CVector(m_YAngle, 0.0, 0.0)) * CMatrix(CVector(0.0, m_XAngle, 0.0));
	m_Position *= m_Orientation;

	//Origin
	if(m_TileCenter)
		m_Position += CVector(0.0, VERTSIZE/2, 0.0);

}
