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

CPhysics::CPhysics(const CLConfig *conf)
{
	m_Detector = new CCollisionDetector;
	m_FastCPUMode = true; //start mode; not really important

	m_dtMin = conf->getValue("simulation", "dtmin").toFloat();
	m_NMax = conf->getValue("simulation", "nmax").toInt();
}

CPhysics::~CPhysics()
{
	delete m_Detector;
}

bool CPhysics::update()
{
	if(!(theWorld->m_Paused))
	{
		float dt = m_Timer.getdt(m_dtMin + 0.0001);

#ifdef DEBUGMSG
		if(dt > 0.5)
			{printf("Warning: Low update time detected\n"); dt = 0.5;}
#endif

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

		if(N == 0) N = 1; //should not be neccesary

		vector<CDataObject *> objs = theWorld->getObjectArray(CDataObject::eMovingObject);
		for(unsigned int step=0; step < N; step++)
		{
			vector<vector<CCollisionData> > collisions;
			for(unsigned int i=0; i < objs.size(); i++)
			{
				((CMovingObject *)objs[i])->update(this, dt);

				collisions.push_back(m_Detector->getCollisions((CMovingObject *)objs[i]));
			}

			for(unsigned int i=0; i < objs.size(); i++)
			{
				vector<CCollisionData> &cols = collisions[i];
				
				for(unsigned int c=0; c < cols.size(); c++)
				{
					CMovingObject *mo = (CMovingObject *)objs[i];
					CCollisionData col = cols[c];

					//printf("depth = %.3f\n", col.depth);

					//correct the position
					mo->m_Position += col.nor * col.depth;

					//set the collision velocity to zero
					float radcomp = mo->m_Velocity.dotProduct(col.nor);
					if(radcomp < 0.0)
						mo->m_Velocity -= radcomp * col.nor;
				}
			}
		}
	}
	return true;
}

const CCollisionFace *CPhysics::getGroundFace(const CVector &pos)
{
	return m_Detector->getGroundFace(pos);
}

