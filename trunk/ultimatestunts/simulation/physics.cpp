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
#include "carinput.h"
#include "usmacros.h"
#include "generalmatrix.h"

#ifndef M_PI
#define M_PI 3.1415926536
#endif

#define g 9.81

CPhysics::CPhysics(CWorld *w) : CSimulation(w)
{}

CPhysics::~CPhysics(){
}

bool CPhysics::update()
{
	float dt = m_Timer.getdt(0.01); //max. 100 fps

#ifdef DEBUGMSG
	if(dt > 0.5)
		{printf("Warning: Low update time detected\n"); dt = 0.5;}
	if(dt < 0.0001)
	{
		printf("Physics debugging message: dt = %f\n", dt);
	}
#endif

#define cwA 10
#define gasmax 25000
#define remmax 10000

	//Standard movement + rotation
	for(unsigned int i=0; i < (m_World->m_MovObjs.size()); i++)
	{
		CMovingObject *mo =m_World->m_MovObjs[i];
		mo->rememberCurrentState();
		mo->setPosition(mo->getPosition() + mo->getVelocity() * dt);
		mo->setOrientationVector(mo->getOrientationVector() + mo->getAngularVelocity() * dt);
	}

	//Update body data
	for(unsigned int i=0; i < (m_World->m_MovObjs.size()); i++)
		m_World->m_MovObjs[i]->updateBodyData();

	//Calculate collision info
	m_World->m_CollData->calculateCollisions();

	//Do some physics
	for(unsigned int i=0; i < (m_World->m_MovObjs.size()); i++)
	{
		CMovingObject *mo =m_World->m_MovObjs[i];

		if(mo->getType()==CMessageBuffer::car)
		{
			CCar *theCar = (CCar *)mo;
			CCarInput *input = (CCarInput *)mo->m_InputData;

			//Total force & torque
			CVector Ftot, Mtot;

			CMatrix R = mo->getOrientation();
			const float invmass = mo->m_InvMass;
			const CMatrix &Iinv = mo->getActualInvMomentInertia();

			float gas = input->m_Forward;
			float rem = input->m_Backward;

			//Collision response
			CMatrix Jinv = mo->getActualInvMomentInertia() / mo->m_InvMass;
			if(m_World->m_CollData->m_Events[i].isHit)
			{
				const CColEvents &e = m_World->m_CollData->m_Events[i];
				CVector dp, dL;

				//Set up the matrix as described in usphysics1.pdf
				CGeneralMatrix M(e.size(), e.size());
				CGeneralVector a;
				for(unsigned int j=0; j<e.size(); j++)
				{
					const CCollision &cj = e[j];
					CMatrix Rj; Rj.setCrossProduct(cj.pos);

					for(unsigned int k=0; k<e.size(); k++)
					{
						const CCollision &ck = e[k];
						CMatrix Rk; Rk.setCrossProduct(ck.pos);
						M.setElement(j, k, cj.nor.dotProduct(ck.nor + Rj*(Jinv*(Rk*ck.nor)) ));
					}

					a.push_back(cj.p);

				}

				//get a copy
				CGeneralVector a_copy;
				for(unsigned int j=0; j<a.size(); j++)
					a_copy.push_back(a[j]);
				
				if(m_World->printDebug)
				{
					CVector v = mo->getVelocity();
					CVector p = v / mo->m_InvMass;
					CVector w = mo->getAngularVelocity();
					CVector L = w; L /= mo->getActualInvMomentInertia();
					fprintf(stderr, "v = %.3f, %.3f, %.3f\n", v.x, v.y, v.z);
					fprintf(stderr, "w = %.3f, %.3f, %.3f\n", w.x, w.y, w.z);
					fprintf(stderr, "p = %.3f, %.3f, %.3f\n", p.x, p.y, p.z);
					fprintf(stderr, "L = %.3f, %.3f, %.3f\n", L.x, L.y, L.z);
				}
					
				//Solve it
				M.m_debug = m_World->printDebug;
				int solve_ret = M.solve(&a, GM_INEQUAL);
				
				if(solve_ret == -2)
					{printf("physics.cpp: Matrix size error\n");}
				else if(solve_ret == -1)
				{
					for(unsigned int j=0; j<e.size(); j++)
					{
						//Get the smallest:
						float p;
						if(a[j] < a_copy[j] && a[j] > -0.01)
							{p = a[j];}
						else if (a_copy[j] > 0.0)
							{p = a_copy[j];}
						else
							{p = 0.0;}

						if(m_World->printDebug)
						{
							fprintf(stderr, "a[%d] = %.3f; p = %.3f\n", j, a[j], p);
						}

						dp += p * e[j].nor;
						dL -= p * e[j].pos.crossProduct(e[j].nor);
					}
					
					if(m_World->printDebug)
					{
						fprintf(stderr, "dp = %.3f, %.3f, %.3f\n", dp.x, dp.y, dp.z);
					}
				}
				else
				{
					printf("Solving unsuccesful @ row %d\n", solve_ret);
				}

				Ftot += dp/dt; //collision force
				Mtot += dL/dt; //collision torque


				//Placing back:
				CVector dr_min, dr_max;
				for(unsigned int j=0; j<e.size(); j++)
				{
					const CCollision &cj = e[j];
					CVector dr = cj.nor * cj.penetrationDepth;

					//quick&dirty:
					if(dr.x > dr_max.x) dr_max.x = dr.x;
					if(dr.y > dr_max.y) dr_max.y = dr.y;
					if(dr.z > dr_max.z) dr_max.z = dr.z;
					if(dr.x < dr_min.x) dr_min.x = dr.x;
					if(dr.y < dr_min.y) dr_min.y = dr.y;
					if(dr.z < dr_min.z) dr_min.z = dr.z;
				}
				CVector dr;
				if(dr_max.x > -dr_min.x) dr.x = dr_max.x; else dr.x = dr_min.x;
				if(dr_max.y > -dr_min.y) dr.y = dr_max.y; else dr.y = dr_min.y;
				if(dr_max.z > -dr_min.z) dr.z = dr_max.z; else dr.z = dr_min.z;
				mo->setPosition(mo->getPosition() + dr);
			}


			//Gas
			CVector Fgas = R * CVector(0.0, 0.0, -gasmax*gas);
			Ftot += Fgas;

			//Brake + friction
			CVector v = mo->getVelocity();
			float vabs = v.abs();
			CVector Frem;
			if(vabs < 0.001)
				{Frem = -v;}
			else
				{Frem = v * (-remmax*rem/vabs -cwA*vabs);}
			Ftot += Frem;

			//rotational air friction
			CVector w = mo->getAngularVelocity();
			float wabs = w.abs();
			CVector Mrem;
			if(wabs < 0.001)
				{Mrem = -w;}
			else
				{Mrem = w * (-500.0*cwA*wabs);}
			Mtot += Mrem;

			//Gravity
			Ftot += CVector(0.0, -g / invmass, 0.0);

			//Dummy wheel rotation
			//positive angle = driving forward = positive vz
			theCar->m_WheelVelocity = vabs / theCar->m_WheelRadius;
			theCar->m_WheelAngle += dt * theCar->m_WheelVelocity;

			//standard acceleration:
			mo->setVelocity(mo->getVelocity() + Ftot * dt * invmass);

			//Dummy roteer-functie:
			float stuur = input->m_Right;
			//Mtot += CVector(0,100000.0*stuur*dt,0);
			//standard rotation
			mo->setAngularVelocity(mo->getAngularVelocity() + Iinv * Mtot * dt + CVector(0, 10.0*stuur*dt, 0) );
		}
		else
			{printf("Error: object %d is not a car\n", i);}
	}

	return true;
}
