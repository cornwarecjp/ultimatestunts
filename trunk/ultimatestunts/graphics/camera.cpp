/***************************************************************************
                          camera.cpp  -  A basic camera-class
                             -------------------
    begin                : ma feb 3 2003
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

#include "camera.h"

CCamera::CCamera()
{
	m_Position = CVector(0,0,0);
	m_Orientation.reset();
}
CCamera::~CCamera(){
}

const CVector &CCamera::getPosition() const
{return m_Position;}

const CMatrix &CCamera::getOrientation() const
{return m_Orientation;}
