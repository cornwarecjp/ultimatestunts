/***************************************************************************
                          graphicbound.h  -  a combination of CGraphObj and CBound
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

#ifndef GRAPHICBOUND_H
#define GRAPHICBOUND_H

#include "bound.h"
#include "graphobj.h"

#include "graphicmaterial.h"

/**
  *@author CJP
  */

class CGraphicBound : public CBound, public CGraphObj
{
public:
	virtual bool loadFromFile(CString filename, CMaterial **matarray)
		{return CBound::loadFromFile(filename, matarray) &&
			CGraphObj::loadFromFile(filename, matarray);}
};

#endif
