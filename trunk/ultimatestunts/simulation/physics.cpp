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

#include "physics.h"
#include "car.h"
#include "usmacros.h"

#ifndef M_PI
#define M_PI 3.1415926536
#endif

/*
It is wrong to think that the task of physics is
to find out how nature is. Physics concerns
what we say about nature.

Niels Bohr
*/

CPhysics::CPhysics(CWorld *w) : CSimulation(w)
{
	m_Detector = new CCollisionDetector;
	m_firstTime = true;
}

CPhysics::~CPhysics()
{
	delete m_Detector;
}

bool CPhysics::update()
{
	if(!(theWorld->m_Paused))
	{
		float dt = m_Timer.getdt(0.01); //max. 100 fps

		if(m_firstTime)
		{
			m_firstTime = false;
			return true;
		}
		
#ifdef DEBUGMSG
		if(dt > 0.5)
			{printf("Warning: Low update time detected\n"); dt = 0.5;}
#endif

		for(unsigned int i=0; i < (m_World->m_MovObjs.size()); i++)
		{
			m_CurrentMovingObject = i;
			theWorld->m_MovObjs[i]->update(this, dt);
		}

		m_Detector->calculateCollisions();
		dWorldStep(theWorld->m_ODEWorld, dt);
		dJointGroupEmpty(theWorld->m_ContactGroup);

	}
	return true;
}


