/***************************************************************************
                          graphobj.h  -  Graphics geometry object
                             -------------------
    begin                : Tue Apr 23 2002
    copyright            : (C) 2002 by CJP
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
#ifndef GRAPHOBJ_H
#define GRAPHOBJ_H

#include "texture.h"
#include "cstring.h"

class CGraphObj
{
  public:
		CGraphObj();
	int loadFromFile(CString filename, CTexObj *texarray, int lod);

    void draw();

  protected:
    unsigned int m_ObjList;

    void setMaterialColor(CVector c);
};

#endif

