/***************************************************************************
                          graphicworld.cpp  -  A graphical version of a world object
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
#include <stdio.h>

#include "graphicshape.h"
#include "graphicbound.h"
#include "graphicmaterial.h"

#include "car.h"

#include "graphicworld.h"

CGraphicWorld::CGraphicWorld(){
}
CGraphicWorld::~CGraphicWorld(){
}

CShape *CGraphicWorld::createShape()
{return new CGraphicShape;}

CBound *CGraphicWorld::createBound()
{return new CGraphicBound;}

CMaterial *CGraphicWorld::createMaterial()
{return new CGraphicMaterial;}
