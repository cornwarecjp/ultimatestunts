/***************************************************************************
                          graphicshape.h  -  a combination of CGraphObj and CShape
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

#ifndef GRAPHICSHAPE_H
#define GRAPHICSHAPE_H

#include "shape.h"
#include "graphobj.h"

#include "graphicmaterial.h"

/**
  *@author CJP
  */

class CGraphicShape : public CShape, public CGraphObj
{
public: 
	virtual bool loadFromFile(CString filename, CMaterial **matarray)
		{return CShape::loadFromFile(filename, matarray) &&
		CGraphObj::loadFromFile(filename, (CTexture **)(CGraphicMaterial **)matarray);}
};

#endif
