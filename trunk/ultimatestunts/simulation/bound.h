/***************************************************************************
                          bound.h  -  Plane-based collision model
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

#ifndef BOUND_H
#define BOUND_H

#include "collisionmodel.h"

/**
  *@author CJP
  */

class CBound : public CCollisionModel {
public: 
	CBound();
	~CBound();

	virtual bool loadFromFile(CFile *f, CString subset, CMaterial **matarray);

	vector<CVector> m_Points;

	//More detailed description for wheels
	float m_CylinderWidth, m_CylinderRadius;
	bool isCylinder() const
		{return m_isCylinder;}
	void setCylinder(bool b); //also sets the cylinder parameters

protected:
	bool m_isCylinder;
};

#endif
