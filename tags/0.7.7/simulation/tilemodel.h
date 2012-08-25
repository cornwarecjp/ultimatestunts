/***************************************************************************
                          tilemodel.h  -  The (collision) model of a tile
                             -------------------
    begin                : wo sep 24 2003
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

#ifndef TILEMODEL_H
#define TILEMODEL_H

#include "collisionmodel.h"

/**
  *@author CJP
  */

class CTileModel : public CCollisionModel  {
public: 
	CTileModel(CDataManager *manager);
	virtual ~CTileModel();

	virtual bool load(const CString &filename, const CParamList &list);

	bool m_isStart, m_isFinish;
	float m_Time;
};

#endif
