/***************************************************************************
                          physics.h  -  The physics of the game :-)
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

#ifndef PHYSICS_H
#define PHYSICS_H


/**
  *@author CJP
  */

#include <ode/ode.h>

#include "simulation.h"
#include "timer.h"
#include "collisiondetector.h"
#include "generalmatrix.h"


class CPhysics : public CSimulation {
public: 
	CPhysics(CWorld *w);
	virtual ~CPhysics();

	virtual bool update();

	void addContactForces(unsigned int body, CVector &Ftot, CVector &Mtot);
protected:
	CTimer m_Timer;
	bool m_firstTime;

	void calculateRestingContacts();
	CCollision getFirstCollision();
	void doCollision(CCollision &c);

	unsigned int m_CurrentMovingObject;

	CCollisionDetector *m_Detector;


	//used by addContactForces:
	/*
	vector<CCollision> m_col; //the relevant collisions

	CGeneralVector *m_a, *m_b, *m_f;
	CGeneralMatrix *m_A;
	vector<unsigned int> m_C, m_NC;
	unsigned int m_N;
	void driveToZero(unsigned int d);
	CGeneralVector fdirection(unsigned int d);
	void maxstep(float &s, unsigned int &j, const CGeneralVector &df, const CGeneralVector &da, unsigned int d);
	*/
};

#endif
