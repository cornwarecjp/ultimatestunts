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

static void _nearCallback(void *data, dGeomID o1, dGeomID o2)
{
	bool g1 = (o1 == theWorld->m_Ground);
	bool g2 = (o2 == theWorld->m_Ground);
	if(!(g1 ^ g2)) return;

	const int N = 10;
	dContact contact[N];
	int n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));
	if(n > 0)
	{
		for(int i=0; i<n; i++)
		{
			contact[i].surface.mode = dContactSlip1 | dContactSlip2 | dContactApprox1;
			contact[i].surface.mu = dInfinity;
			contact[i].surface.slip1 = 0.001;
			contact[i].surface.slip2 = 0.001;
			dJointID c = dJointCreateContact(theWorld->m_ODEWorld, theWorld->m_ContactGroup, &contact[i]);
			dJointAttach(c, dGeomGetBody(contact[i].geom.g1), dGeomGetBody(contact[i].geom.g2));
		}
	}
}


/*
It is wrong to think that the task of physics is
to find out how nature is. Physics concerns
what we say about nature.

Niels Bohr
*/

CPhysics::CPhysics(CWorld *w) : CSimulation(w)
{
	m_Detector = new CCollisionDetector(w);
	
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

		dSpaceCollide(theWorld->m_Space, 0, &_nearCallback);
		dWorldStep(theWorld->m_ODEWorld, dt);
		dJointGroupEmpty(theWorld->m_ContactGroup);

	}
	return true;
}

void CPhysics::doCollision(CCollision &c)
{
	/*
	//TODO: per-body forces
	
	if(c.object1 >= 0 && c.body1 >= 0)
	{
		CMovingObject *mo = m_World->m_MovObjs[c.object1];

		CVector objpos = c.t * mo->getPreviousPosition() + (1-c.t) * mo->getPosition();
		CVector r = c.pos - objpos;
		CVector dp = -c.p * c.nor; //substract
		CVector dL = -r.crossProduct(dp);
		mo->setMomentum(mo->getMomentum() + dp);
		mo->setAngularMomentum(mo->getAngularMomentum() + dL);
	}

	if(c.object2 >= 0 && c.body2 >= 0)
	{
		CMovingObject *mo = m_World->m_MovObjs[c.object2];

		CVector objpos = c.t * mo->getPreviousPosition() + (1-c.t) * mo->getPosition();
		CVector r = c.pos - objpos;
		CVector dp = c.p * c.nor; //add
		fprintf(stderr, "c.pos = %.3f,%.3f,%.3f\n", c.pos.x, c.pos.y, c.pos.z);
		fprintf(stderr, "objpos = %.3f,%.3f,%.3f\n", objpos.x, objpos.y, objpos.z);
		fprintf(stderr, "r = %.3f,%.3f,%.3f\n", r.x, r.y, r.z);
		fprintf(stderr, "dp = %.3f,%.3f,%.3f\n", dp.x, dp.y, dp.z);
		CVector dL = -r.crossProduct(dp);
		fprintf(stderr, "dL = %.3f,%.3f,%.3f\n", dL.x, dL.y, dL.z);
		mo->setMomentum(mo->getMomentum() + dp);
		mo->setAngularMomentum(mo->getAngularMomentum() + dL);
	}
	*/
}

CCollision CPhysics::getFirstCollision()
{
	CCollision ret;
	ret.t = 2.0;
	
	m_Detector->calculateCollisions(false); //calculate non-resting contacts
	
	if(m_Detector->m_Collisions.size() == 0) //no collisions
		return ret;

	for(unsigned int i=0; i < m_Detector->m_Collisions.size(); i++)
	{
		CCollision &theColl = m_Detector->m_Collisions[i];
		if((!theColl.isResting) && theColl.t < ret.t && theColl.t > -0.01)
			ret = theColl;
	}

	return ret;
}

void CPhysics::calculateRestingContacts()
{
	m_Detector->calculateCollisions(true); //calculate resting contacts
}

void CPhysics::addContactForces(unsigned int body, CVector &Ftot, CVector &Mtot)
{
	;
}

