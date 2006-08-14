/***************************************************************************
                          staticreflection.h  -  description
                             -------------------
    begin                : za mei 14 2005
    copyright            : (C) 2005 by CJP
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

#ifndef STATICREFLECTION_H
#define STATICREFLECTION_H

#include "reflection.h"
#include "texture.h"

/**
  *@author CJP
  */

class CStaticReflection : public CReflection , public CTexture {
public: 
	CStaticReflection(CDataManager *manager);
	~CStaticReflection();

	bool load(const CString &filename, const CParamList &list);
};

#endif
