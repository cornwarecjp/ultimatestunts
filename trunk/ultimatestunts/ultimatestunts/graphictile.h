/***************************************************************************
                          graphictile.h  -  A graphic CTile class. Same purpose as the tile class in v.0.2.3
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

#ifndef GRAPHICTILE_H
#define GRAPHICTILE_H

#include "tile.h"
#include "graphicobject.h"

/**
  *@author CJP
  */

class CGraphicTile : public CTile, public CGraphicObject
{
public: 
	CGraphicTile();
	virtual ~CGraphicTile();

};

#endif
