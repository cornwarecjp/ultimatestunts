/***************************************************************************
                          shape.h  -  Vertex-based collision model
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

#ifndef SHAPE_H
#define SHAPE_H

#include "material.h"
#include "cstring.h"

/**
  *@author CJP
  */

class CShape {
public: 
	CShape();
	virtual ~CShape();

	virtual bool loadFromFile(CString filename, CString subset, CMaterial ** matarray);

	CString getFilename()
		{return m_Filename;}

	CString getSubset()
		{return m_Subset;}

protected:
	CString m_Filename;
	CString m_Subset;
};

#endif
