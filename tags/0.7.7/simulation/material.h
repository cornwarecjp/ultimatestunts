/***************************************************************************
                          material.h  -  Friction coefficients etc.
                             -------------------
    begin                : ma jan 27 2003
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

#ifndef MATERIAL_H
#define MATERIAL_H

#include "dataobject.h"
#include "cstring.h"

/**
  *@author CJP
  */

class CMaterial : public CDataObject {
public:
	CMaterial(CDataManager *manager);
	virtual ~CMaterial();

	virtual bool load(const CString &filename, const CParamList &list);

	int m_Mul; //texture size multiplier
	float m_Mu; //static friction coefficient
	float m_Roll; //roll friction coefficient
};

#endif
