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

void CBody::updateFixedBounds(const CCollisionModel& coll)
{
	m_FBB_min = coll.m_OBB_min;
	m_FBB_max = coll.m_OBB_max;
}
