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

#define DBLPI 6.283185307

CMovingObject::CMovingObject(CDataManager *manager) : CDataObject(manager, CDataObject::eMovingObject)
{
	m_InputData = new CMovObjInput;
}

CMovingObject::~CMovingObject()
{
	delete m_InputData; //I guess this will happen for all CMovingObject-derived classes

	for(unsigned int i=0; i < m_Bodies.size(); i++)
		m_Bodies[i].destroyODE();
}

void CMovingObject::update(CPhysics *simulator, float dt)
{
}

CBinBuffer &CMovingObject::getData(CBinBuffer &b) const
{
	//b.addVector32(m_Position, 0.001);
	//b.addVector16(m_Momentum, 50.0);
	//b.addVector16(m_Orientation, 0.0001);
	//b.addVector16(m_AngularMomentum, 15.0);
	return b;
}

bool CMovingObject::setData(const CBinBuffer &b)
{
	//unsigned int pos = 0;
	//setPosition(b.getVector32(pos, 0.001));
	//m_Momentum = b.getVector16(pos, 50.0);
	//setOrientation(b.getVector16(pos, 0.0001));
	//m_AngularMomentum = b.getVector16(pos, 15.0);
	return true;
}

bool CMovingObject::hasChanged()
{
	return true;
}
