/***************************************************************************
                          physics.cpp  -  The physics of the game :-)
                             -------------------
    begin                : Wed Nov 20 2002
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
#include <cmath>

#include "pi.h"

#include "physics.h"
#include "car.h"
#include "usmacros.h"

/*
It is wrong to think that the task of physics is
to find out how nature is. Physics concerns
what we say about nature.

Niels Bohr
*/

CPhysics::CPhysics(const CLConfig *conf)
{
	m_FastCPUMode = true; //start mode; not really important

	m_dtMin = conf->getValue("simulation", "dtmin").toFloat();
	m_NMax = conf->getValue("simulation", "nmax").toInt();
}

CPhysics::~CPhysics()
{
}

bool CPhysics::update()
{
	float dt = theWorld->m_Lastdt;
	vector<CDataObject *> objs = theWorld->getObjectArray(CDataObject::eMovingObject);


	for(unsigned int i=0; i < objs.size(); i++)
	{
		CMovingObject *mo = (CMovingObject *)objs[i];
		mo->m_LastUpdateTime = theWorld->m_LastTime;
	}

	//Skip the rest if paused:
	if(theWorld->m_Paused) return true;

	float dtreal = dt;
	unsigned int N = 0;

	if(m_FastCPUMode)
	{
		N = (unsigned int)(dtreal / m_dtMin);
		dt = m_dtMin;

		if(N > m_NMax)
		{
			m_FastCPUMode = false;
			N = m_NMax;
			dt = dtreal / m_NMax;
		}
	}
	else
	{
		N = m_NMax;
		dt = dtreal / m_NMax;

		if(dt < m_dtMin)
		{
			m_FastCPUMode = true;
			N = (unsigned int)(dtreal / m_dtMin);
			dt = m_dtMin;
		}
	}

	if(N == 0) N = 1; //should not be necesary

	//clear the collision arrays:
	for(unsigned int i=0; i < objs.size(); i++)
		((CMovingObject *)objs[i])->m_AllCollisions.clear();
	
	for(unsigned int step=0; step < N; step++)
	{
		//simulation
		for(unsigned int i=0; i < objs.size(); i++)
		{
			CMovingObject *mo = (CMovingObject *)objs[i];
			mo->update(this, dt);
		}

		//collision detection
		for(unsigned int i=0; i < objs.size(); i++)
		{
			CMovingObject *mo = (CMovingObject *)objs[i];
			mo->m_SimCollisions = theWorld->m_Detector.getCollisions(mo);
		}

		//collision response
		for(unsigned int i=0; i < objs.size(); i++)
			((CMovingObject *)objs[i])->correctCollisions();

		//Add to m_AllCollisions
		for(unsigned int i=0; i < objs.size(); i++)
		{
			CMovingObject *mo = (CMovingObject *)objs[i];
			for(unsigned int j=0; j < mo->m_SimCollisions.size(); j++)
				mo->m_AllCollisions.push_back(mo->m_SimCollisions[j]);
		}
	}

	return true;
}
