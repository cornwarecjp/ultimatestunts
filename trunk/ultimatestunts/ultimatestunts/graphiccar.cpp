/***************************************************************************
                          graphiccar.cpp  -  A graphical version of a car object
                             -------------------
    begin                : do jan 16 2003
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

#include "graphiccar.h"

CGraphicCar::CGraphicCar()
{
}

CGraphicCar::~CGraphicCar()
{
}

void CGraphicCar::draw()
{
	m_Body->draw();
	//TODO: draw other parts on the right position
}
