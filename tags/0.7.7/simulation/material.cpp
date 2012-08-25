/***************************************************************************
                          material.cpp  -  Friction coefficients etc.
                             -------------------
    begin                : vr dec 3 2004
    copyright            : (C) 2004 by CJP
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

#include "material.h"

CMaterial::CMaterial(CDataManager *manager) : CDataObject(manager, CDataObject::eMaterial)
{
	m_Mul = 1;
	m_Mu = 1.0;
	m_Roll = 1.0;
}

CMaterial::~CMaterial()
{}

bool CMaterial::load(const CString &filename, const CParamList &list)
{
	CDataObject::load(filename, list);

	m_Mul  = m_ParamList.getValue("scale", "1"  ).toInt();
	m_Mu   = m_ParamList.getValue("mu"   , "1.0").toFloat();
	m_Roll = m_ParamList.getValue("roll" , "1.0").toFloat();

	return true;
}
