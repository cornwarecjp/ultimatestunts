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
#include <stdio.h>
#include <math.h>

#include "physics.h"
#include "car.h"
#include "carinput.h"

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
	float dt = m_Timer.getdt();
	//if(dt > 1.0)
	//	{printf("Warning: Low update time detected\n"); dt = 1.0;}
	if(dt < 0.0001)
	{
		printf("Physics debugging message: dt = %f\n", dt);
	}

#define cwA 10
#define gasmax 10000
#define remmax 10000
#define invstraal 0.05

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

			//Total force
			CVector Ftot;

			CMatrix R = mo->getRotationMatrix();
			float m = mo->m_Mass;

			float gas = input->m_Forward;
			float rem = input->m_Backward;

			CVector r = mo->getPosition();
			CVector v = mo->getVelocity();
			float vabs = v.abs();

			//Collision response
			if(m_World->m_CollData->m_Events[i].isHit)
			{
				printf("Object %d is hit\n", i);
				CColEvents e = m_World->m_CollData->m_Events[i];
				for(unsigned int j=0; j<e.size(); j++)
				{
					CCollision c = e[j];
					r += c.dr; //place back

					CVector Fcol = c.dp/dt; //collision force
					Ftot += Fcol;
				}
			}


			//Gas
			CVector Fgas = CVector(0.0, 0.0, -gasmax*gas) * R;
			Ftot += Fgas;

			//Brake + friction
			CVector Frem;
			if(vabs < 0.001)
				{Frem = -v;}
			else
				{Frem = v * (-(remmax*rem + cwA*v.abs2()) / vabs);}
			Ftot += Frem;

			//Gravity
			Ftot += CVector(0.0, -g*m, 0.0);

			//Dummy wheel rotation
			//positive angle = driving forward = positive vz
			theCar->m_WheelVelocity = vabs / theCar->m_WheelRadius;
			theCar->m_WheelAngle += dt * theCar->m_WheelVelocity;

			//Dummy roteer-functie:
			float stuur = input->m_Right;
			CMatrix dR;
			dR.rotY(stuur*invstraal*vabs*dt);
			R *= dR;
			mo->setRotationMatrix(R);

			//standard acceleration:
			v += Ftot * dt/m;
			mo->setVelocity(v);
			r += v * dt;
			mo->setPosition(r);
		}
		else
			{printf("Error: object %d is not a car\n", i);}
	}

	return true;
}
