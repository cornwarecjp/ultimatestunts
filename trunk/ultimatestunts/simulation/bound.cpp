/***************************************************************************
                          bound.cpp  -  Plane-based collision model
                             -------------------
    begin                : vr jan 24 2003
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

#include "bound.h"

CBound::CBound(){
}
CBound::~CBound(){
}

bool CBound::loadFromFile(CString filename, CMaterial **matarray)
{
	m_Filename = filename;

	m_BSphere_r = 5.0;
	m_BBox_min = CVector(-2.0,-1.0,-2.5);
	m_BBox_max = CVector(2.0,1.0,2.5);

	return true; //TODO: really load it
}
