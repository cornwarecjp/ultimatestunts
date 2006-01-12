/***************************************************************************
                          approximation.cpp  -  Approximate physics behavior between network updates
                             -------------------
    begin                : do dec 15 2005
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

#include <cstdio>

#include "approximation.h"

CApproximation::CApproximation()
{
}

CApproximation::~CApproximation()
{
}

bool CApproximation::update()
{
	vector<CDataObject *> objs = theWorld->getObjectArray(CDataObject::eMovingObject);

	float dt = theWorld->m_Lastdt;

	for(unsigned int i=0; i < objs.size(); i++)
	{
		/*
		A very simple approximation: just extrapolate the
		positions according to the velocities
		*/
			
		CMovingObject *mo = (CMovingObject *)objs[i];

		mo->m_LastUpdateTime = theWorld->m_LastTime;

		mo->m_Position += mo->m_Velocity * dt;

		CMatrix dM;
		dM.setRotation(mo->m_AngularVelocity * dt);
		mo->m_OrientationMatrix *= dM;

		mo->resetBodyPositions();
	}

	return true;
}
