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
	m_Detector = new CCollisionDetector(w);
}

CPhysics::~CPhysics()
{
	delete m_Detector;
}

bool CPhysics::update()
{
	float dt = m_Timer.getdt(0.01); //max. 100 fps

#ifdef DEBUGMSG
	if(dt > 0.5)
		{printf("Warning: Low update time detected\n"); dt = 0.5;}
#endif

	//Create backup state array
	vector<CBinBuffer> backupState;
	for(unsigned int i=0; i < (m_World->m_MovObjs.size()); i++)
	{
		CBinBuffer b;
		backupState.push_back(b);
	}

	m_World->m_Collisions.clear();

	while(true)
	{
		//backup state
		for(unsigned int i=0; i < (m_World->m_MovObjs.size()); i++)
		{
			m_World->m_MovObjs[i]->rememberCurrentState(); //for collision detection
			m_World->m_MovObjs[i]->getData(backupState[i]);
		}

		//Calculate the resting contacts
		calculateRestingContacts();

		/*
		{
			CMovingObject *mo = m_World->m_MovObjs[0];
			CVector p = mo->getMomentum();
			fprintf(stderr, "Before simulation step:\n");
			fprintf(stderr, "p = %.3f, %.3f, %.3f\n", p.x, p.y, p.z);
		}
		*/
		
		//Simulate towards dt
		for(unsigned int i=0; i < (m_World->m_MovObjs.size()); i++)
		{
			m_CurrentMovingObject = i;
			m_World->m_MovObjs[i]->update(this, dt);
		}

		/*
		{
			CMovingObject *mo = m_World->m_MovObjs[0];
			CVector p = mo->getMomentum();
			fprintf(stderr, "After simulation step:\n");
			fprintf(stderr, "p = %.3f, %.3f, %.3f\n", p.x, p.y, p.z);
		}
		*/

		//Get the first collision
		CCollision fc = getFirstCollision();

		if(fc.t > 1.1 || fc.t < -0.1) break;//no collision or invalid collision
		m_World->m_Collisions.push_back(fc);

		//restore state
		for(unsigned int i=0; i < (m_World->m_MovObjs.size()); i++)
			m_World->m_MovObjs[i]->setData(backupState[i]);

		{
			CMovingObject *mo = m_World->m_MovObjs[0];
			CVector p = mo->getMomentum();
			fprintf(stderr, "We have a collision @ t = %.3f\n", fc.t);
			fprintf(stderr, "Restored state:\n");
			fprintf(stderr, "p = %.3f, %.3f, %.3f\n", p.x, p.y, p.z);
		}


		//Calculate the resting contacts
		calculateRestingContacts();

		//Simulate towards dtcoll
		float dtcoll = fc.t * dt;
		for(unsigned int i=0; i < (m_World->m_MovObjs.size()); i++)
			m_World->m_MovObjs[i]->update(this, dtcoll);

		{
			CMovingObject *mo = m_World->m_MovObjs[0];
			CVector p = mo->getMomentum();
			fprintf(stderr, "After first simulation:\n");
			fprintf(stderr, "p = %.3f, %.3f, %.3f\n", p.x, p.y, p.z);
		}

		doCollision(fc);

		{
			CMovingObject *mo = m_World->m_MovObjs[0];
			CVector p = mo->getMomentum();
			fprintf(stderr, "After the collision:\n");
			fprintf(stderr, "p = %.3f, %.3f, %.3f\n", p.x, p.y, p.z);
		}

		//Simulate the rest in the next loop
		dt -= dtcoll;
	}
	
	return true;
}

void CPhysics::doCollision(CCollision &c)
{
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

#define VERY_SMALL 0.0001

void CPhysics::addContactForces(unsigned int body, CVector &Ftot, CVector &Mtot)
{
	if(m_Detector->m_Collisions.size() == 0) return;

	//TODO
	//use m_CurrentMovingObject

	CMovingObject *mo = m_World->m_MovObjs[m_CurrentMovingObject];
	CVector bodypos = mo->getPosition() + mo->m_Bodies[body].m_Position;

	//Collect the relevant collisions
	m_col.clear();
	for(unsigned int i=0; i < m_Detector->m_Collisions.size(); i++)
	{
		CCollision c = m_Detector->m_Collisions[i];
		if(c.object1 == (int)m_CurrentMovingObject && c.body1 == (int)body)
			{;}
		else if(c.object2 == (int)m_CurrentMovingObject && c.body2 == (int)body)
		{
			int o=c.object1,b=c.body1,m=c.mat1; //swap the bodies
			c.object1 = c.object2;
			c.body1 = c.body2;
			c.mat1 = c.mat2;
			c.nor = -c.nor;
			c.object2 = o;
			c.body2 = b;
			c.mat2 = m;
		}
		else
			{continue;} //this collision is irrelevant

		//relative to the body position:
		c.pos -= bodypos;
		m_col.push_back(c);
	}
	m_N = m_col.size();

	//Now remove near-equal collisions, to prevent linear dependent rows
	//that could give errors in matrix solving
	/*
	if(m_N > 1)
		for(unsigned int i=0; i < m_N-1; i++)
			for(unsigned int j=i+1; j < m_N; j++)
			{
				float dp1 = m_col[i].nor.dotProduct(m_col[j].nor);
				float dp2 = m_col[i].pos.dotProduct(m_col[j].pos) / (m_col[i].pos.abs() * m_col[j].pos.abs());
				if(dp1 > 0.999 && dp2 > 0.999)
				{
					m_col.erase(m_col.begin() + j);
					m_N--;
					j--;
					continue;
				}
			}
	*/

	if(m_N == 0) return;

	/*
	The following algorithm is an implementation of the normal force
	determination method described in sig94.pdf

	The basic problem is described as a matrix equation
	a = Af + b
	where a is a vector containing the normal acceleration in each
	contact point, A is a matrix, f is a vector containing the normal
	force in each contact point, and b is a vector which describes the
	acceleration of the contact points due to other forces

	A and b are known constants, so let's calculate them first:
	*/
	
	m_b = new CGeneralVector;
	for(unsigned int i=0; i < m_N; i++)
	{
		CVector ni = m_col[i].nor;
		CVector ri = m_col[i].pos;
		m_b->push_back(
			-ni.dotProduct(mo->getInvMass() * Ftot + ri.crossProduct(mo->getActualInvMomentInertia() * Mtot))
			);
	}


	m_A = new CGeneralMatrix(m_N, m_N);
	for(unsigned int i=0; i < m_N; i++)
		for(unsigned int j=0; j < m_N; j++)
		{
			CVector ni = m_col[i].nor;
			CVector nj = m_col[j].nor;
			CVector ri = m_col[i].pos;
			CVector rj = m_col[j].pos;
			m_A->setElement(i, j,
				ni.dotProduct(mo->getInvMass() * nj + ri.crossProduct(mo->getActualInvMomentInertia() * nj.crossProduct(rj)))
				);
		}

	fprintf(stderr, "Going to solve the normal force problem for %d contacts\n", m_N);
	fprintf(stderr, "Initial Ftot = %s\tMtot = %s\n", CString(Ftot).c_str(), CString(Mtot).c_str());
	fprintf(stderr, "\nThese are the collision points:\n");
	for(unsigned int i=0; i < m_N; i++)
		fprintf(stderr, "pos = %s\t\tnor = %s\n", CString(m_col[i].pos).c_str(), CString(m_col[i].nor).c_str() );


	fprintf(stderr, "\n1000*A and b:\n");
	for(unsigned int i=0; i < m_N; i++)
	{
		for(unsigned int j=0; j < m_N; j++)
			fprintf(stderr, "%.3f\t", 1000.0*m_A->getElement(i, j));

		fprintf(stderr, "\t\t%.3f\n", (*m_b)[i]);
	}

	//Now comes the main algorithm:

	m_f = new CGeneralVector(m_N); //all zeroes
	m_a = new CGeneralVector(*m_b); //copy of b
	m_C.clear();
	m_NC.clear();
	/*
	while(true)
	{
		bool d_exists = false;
		unsigned int d = 0;
		for(; d < m_N; d++)
			if((*m_a)[d] < 0.0)
				{d_exists = true; break;}

		if(d_exists)
			{driveToZero(d);} //make it zero
		else
			{break;} //finished: all >= zero
	}
	*/
	for(unsigned int d=0; d < m_N; d++)
		if((*m_a)[d] < 0.0)
			driveToZero(d);
	
	fprintf(stderr, "\nEverything is fine now; exited the solving loop\n\n");


	//Applying the normal forces:
	for(unsigned int ii=0; ii < m_C.size(); ii++)
	{
		unsigned int i = m_C[ii];

		fprintf(stderr, "Contact %d: force %.3f\n", i, (*m_f)[i]);

		Ftot -= (*m_f)[i] * m_col[i].nor;
		Mtot -= (*m_f)[i] * m_col[i].pos.crossProduct(m_col[i].nor);
	}
    
	//Clearing up some stuff:
	delete m_a;
	delete m_b;
	delete m_f;
	delete m_A;
}

void CPhysics::driveToZero(unsigned int d)
{
	fprintf(stderr, "\ndriveToZero(%d)\n", d);
	
	while(true) //L1: in sig94.pdf, but I don't like goto
	{
		fprintf(stderr, "     f =\t");
		for(unsigned int i=0; i < m_N; i++)
			fprintf(stderr, "%.3f\t", (*m_f)[i]);
		fprintf(stderr, "\n1000*a =\t");
		for(unsigned int i=0; i < m_N; i++)
			fprintf(stderr, "%.3f\t", 1000.0*(*m_a)[i]);
		fprintf(stderr, "\n");

		CGeneralVector df = fdirection(d);
		if(df.size() == 0) //matrix solving failed: ignore this contact
		{
			fprintf(stderr, "Matrix solving failed\n");
			break;
		}
		
		CGeneralVector da = (*m_A) * df;

		//force C elements of a to be zero
		for(unsigned int i=0; i < m_C.size(); i++)
			da[m_C[i]] = 0.0;

		fprintf(stderr, "     df =\t");
		for(unsigned int i=0; i < m_N; i++)
			fprintf(stderr, "%.3f  ", df[i]);
		fprintf(stderr, "\n1000*da =\t");
		for(unsigned int i=0; i < m_N; i++)
			fprintf(stderr, "%.3f\t", 1000.0*da[i]);
		fprintf(stderr, "\n");

		float s;
		unsigned int j;
		maxstep(s, j, df, da, d);
		if(s < 0.0)
			fprintf(stderr, "CPhysics::driveToZero: WARNING: s < 0.0\n");
		
		fprintf(stderr, "maxstep returned s = %.3f, j = %d\n", s, j);

		//(*m_f) += df * s;
		//(*m_a) += da * s;
		for(unsigned int i=0; i < m_N; i++)
		{
			(*m_f)[i] += s * df[i];
			(*m_a)[i] += s * da[i];
		}

		fprintf(stderr, "     new f =\t");
		for(unsigned int i=0; i < m_N; i++)
			fprintf(stderr, "%.3f  ", (*m_f)[i]);
		fprintf(stderr, "\nnew 1000*a =\t");
		for(unsigned int i=0; i < m_N; i++)
			fprintf(stderr, "%.3f\t", 1000.0*(*m_a)[i]);
		fprintf(stderr, "\n");

		bool jinC = false;
		unsigned int jindex = 0;
		for(; jindex < m_C.size(); jindex++)
			if(m_C[jindex] == j)
				{jinC = true; break;}
		if(jinC)
		{
			fprintf(stderr, "j in C: moving it to NC\n");
			m_C.erase(m_C.begin() + jindex);
			m_NC.push_back(j);
			(*m_f)[j] = 0.0;
		}
		else
		{
			bool jinNC = false;
			for(jindex = 0; jindex < m_NC.size(); jindex++)
				if(m_NC[jindex] == j)
					{jinNC = true; break;}
			if(jinNC)
			{
				fprintf(stderr, "j in NC: moving it to C\n");
				m_NC.erase(m_NC.begin() + jindex);
				m_C.push_back(j);
				(*m_a)[j] = 0.0;
			}
			else //j must be d, implying ad = 0
			{
				fprintf(stderr, "j must be d: placing it in C\n");
				m_C.push_back(j);
				(*m_a)[j] = 0.0;
				break;
			}
		}
	}
}

CGeneralVector CPhysics::fdirection(unsigned int d)
{
	CGeneralVector df(m_N); //fill with zeroes
	df[d] = 1.0;
	unsigned int Csize = m_C.size();

	//The 1000.0 factors are here to put the matrix & vector
	//elements near 1, so that the solving algorithm will not
	//get too small values

	CGeneralMatrix A11(Csize, Csize);
	for(unsigned int i=0; i < Csize; i++)
		for(unsigned int j=0; j < Csize; j++)
			A11.setElement(i, j, 1000.0*m_A->getElement(m_C[i], m_C[j]));

	fprintf(stderr, "    A11 =\n");
	for(unsigned int i=0; i < Csize; i++)
	{
		fprintf(stderr, "    ");
		for(unsigned int j=0; j < Csize; j++)
			fprintf(stderr, "%.3f  ", A11.getElement(i, j));
		fprintf(stderr, "\n");
	}

	CGeneralVector v1;
	for(unsigned int i=0; i < Csize; i++)
		v1.push_back(-1000.0*m_A->getElement(m_C[i], d));

	fprintf(stderr, "    -v1 = ");
	for(unsigned int i=0; i < Csize; i++)
		fprintf(stderr, "%.3f  ", v1[i]);
	fprintf(stderr, "\n");

	if(A11.solve(&v1) >= 0) //matrix solving failed
	{
		df.clear();
		return df;
	}

	fprintf(stderr, "    x = ");
	for(unsigned int i=0; i < Csize; i++)
		fprintf(stderr, "%.3f  ", v1[i]);
	fprintf(stderr, "\n");

	for(unsigned int i=0; i < Csize; i++)
		df[m_C[i]] = v1[i];

	return df;
}

void CPhysics::maxstep(float &s, unsigned int &j, const CGeneralVector &df, const CGeneralVector &da, unsigned int d)
{
	s = 1234567890;

	if(da[d] > 0.0)
	{
		s = -(*m_a)[d] / da[d];
		j = d;
	}
	//fprintf(stderr, "    maxstep start s = %.3f j = %d\n", s, j);

	for(unsigned int ii=0; ii < m_C.size(); ii++)
	{
		unsigned int i = m_C[ii];
		if(df[i] < 0.0)
		{
			float s2 = -(*m_f)[i] / df[i];
			//fprintf(stderr, "    C: %d: s2 = %.3f\n", i, s2);
			if(s2 < s && s2 > 0.0)
			{
				s = s2;
				j = i;
				//fprintf(stderr, "    new s = %.3f j = %d\n", s, j);
			}
		}
	}

	for(unsigned int ii=0; ii < m_NC.size(); ii++)
	{
		unsigned int i = m_NC[ii];
		if(da[i] < 0.0)
		{
			float s2 = -(*m_a)[i] / da[i];
			//fprintf(stderr, "    NC: %d: s2 = %.3f\n", i, s2);
			if(s2 < s && s2 > 0.0)
			{
				s = s2;
				j = i;
				//fprintf(stderr, "    new s = %.3f j = %d\n", s, j);
			}
		}
	}
}
