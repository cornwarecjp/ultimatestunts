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
}

CMaterial::~CMaterial()
{}

bool CMaterial::load(const CString &idstring)
{
	CDataObject::load(idstring);

	CString line = idstring;

	int pos = line.inStr("scale=");
	if(pos  > 0) //scale is specified
	{
		if((unsigned int)pos < line.length()-6)
			{m_Mul = line.mid(pos+6, line.length()-pos-6).toInt();}
		//TODO: check for different y-direction mul
	}

	pos = line.inStr("mu=");
	if(pos  > 0) //mu is specified
	{
		if((unsigned int)pos < line.length()-3)
			{m_Mu = line.mid(pos+3, line.length()-pos-3).toFloat();}
	}

	pos = line.inStr(' ');
	if(pos  > 0) //there is a space
		line = line.mid(0, pos);

	m_Filename = line;

	return true;
}
