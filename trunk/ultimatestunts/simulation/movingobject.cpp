/***************************************************************************
                          movingobject.cpp  -  Moving object class
                             -------------------
    begin                : Wed Dec 4 2002
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
#include <cstdio>

#include "movingobject.h"
#include "movobjinput.h"
#include "physics.h"

CMovingObject::CMovingObject(CDataManager *manager) : CDataObject(manager, CDataObject::eMovingObject)
{
	m_InputData = new CMovObjInput;
}

CMovingObject::~CMovingObject()
{
	delete m_InputData; //I guess this will happen for all CMovingObject-derived classes
}

bool CMovingObject::load(const CString &filename, const CParamList &list)
{
	CDataObject::load(filename, list);

	m_MovObjID = list.getValue("ID", "0").toInt();
	m_InputData->m_MovObjID = m_MovObjID;

	return true;
}

void CMovingObject::unload()
{
	for(unsigned int i=0; i < m_Bodies.size(); i++)
		m_Bodies[i].destroyODE();
	m_Bodies.clear();

	m_Sounds.clear();
	m_Textures.clear();

	CDataObject::unload();
}

void CMovingObject::update(CPhysics *simulator, float dt)
{
}

CBinBuffer &CMovingObject::getData(CBinBuffer &b) const
{
	b += (Uint8)m_MovObjID;

	for(unsigned int i=0; i < m_Bodies.size(); i++)
	{
		CVector
			p = m_Bodies[i].getPosition(),
			v = m_Bodies[i].getVelocity(),
			o = m_Bodies[i].getOrientationVector(),
			w = m_Bodies[i].getAngularVelocity();

		b.addVector32(p, 0.001);
		b.addVector16(v, 0.01);
		b.addVector16(o, 0.0002);
		b.addVector16(w, 0.01);
	}

	return b;
}

bool CMovingObject::setData(const CBinBuffer &b, unsigned int &pos)
{
	Uint8 ID = b.getUint8(pos);
	if(ID != m_MovObjID) return false;

	//TODO: check the number of objects
	for(unsigned int i=0; i < m_Bodies.size(); i++)
	{
		CVector
			p = b.getVector32(pos, 0.001),
			v = b.getVector16(pos, 0.01),
			o = b.getVector16(pos, 0.0002),
			w = b.getVector16(pos, 0.01);

		m_Bodies[i].setPosition(p);
		m_Bodies[i].setVelocity(v);
		m_Bodies[i].setOrientationVector(o);
		m_Bodies[i].setAngularVelocity(w);
	}

	return true;
}
