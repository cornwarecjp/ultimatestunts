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
#include "usmacros.h"

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
	if(dt > 1.0)
		{printf("Warning: Low update time detected\n"); dt = 1.0;}
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
			const float m = mo->m_Mass;
			const CMatrix &Iinv = mo->m_InvMomentInertia;

			float gas = input->m_Forward;
			float rem = input->m_Backward;

			//Collision response
			if(m_World->m_CollData->m_Events[i].isHit)
			{
				CVector dr_min, dp_min, dr_max, dp_max;
				CVector dL; //rotational momentum change
				const CColEvents &e = m_World->m_CollData->m_Events[i];
				for(unsigned int j=0; j<e.size(); j++)
				{
					const CCollision &c = e[j];
					if(c.body != 0) continue; //TODO: multiple bodies

					//quick&dirty:
					if(c.dr.x > dr_max.x) dr_max.x = c.dr.x;
					if(c.dr.y > dr_max.y) dr_max.y = c.dr.y;
					if(c.dr.z > dr_max.z) dr_max.z = c.dr.z;
					if(c.dr.x < dr_min.x) dr_min.x = c.dr.x;
					if(c.dr.y < dr_min.y) dr_min.y = c.dr.y;
					if(c.dr.z < dr_min.z) dr_min.z = c.dr.z;

					if(c.dp.x > dr_max.x) dp_max.x = c.dp.x;
					if(c.dp.y > dr_max.y) dp_max.y = c.dp.y;
					if(c.dp.z > dr_max.z) dp_max.z = c.dp.z;
					if(c.dp.x < dr_min.x) dp_min.x = c.dp.x;
					if(c.dp.y < dr_min.y) dp_min.y = c.dp.y;
					if(c.dp.z < dr_min.z) dp_min.z = c.dp.z;

					dL += 0.1 * c.dp.crossProduct(c.pos); //TODO: remove 0.1
				}

				//quick&dirty:
				CVector dr, dp;
				dr.x = (dr_max.x > -dr_min.x)? dr_max.x : dr_min.x;
				dr.y = (dr_max.y > -dr_min.y)? dr_max.y : dr_min.y;
				dr.z = (dr_max.z > -dr_min.z)? dr_max.z : dr_min.z;
				dp.x = (dp_max.x > -dp_min.x)? dp_max.x : dp_min.x;
				dp.y = (dp_max.y > -dp_min.y)? dp_max.y : dp_min.y;
				dp.z = (dp_max.z > -dp_min.z)? dp_max.z : dp_min.z;

				mo->setPosition(mo->getPosition() + dr); //place back
				Ftot += dp/dt; //collision force
				Mtot += dL/dt; //collision torque
			}


			//Gas
			CVector Fgas = CVector(0.0, 0.0, -gasmax*gas) * R;
			Ftot += Fgas;

			//Brake + friction
			CVector v = mo->getVelocity();
			float vabs = v.abs();
			CVector Frem;
			if(vabs < 0.001)
				{Frem = -v;}
			else
				{Frem = v * (-(remmax*rem + cwA*v.abs2()) / vabs);}
			Ftot += Frem;

			//rotational air friction
			CVector w = mo->getAngularVelocity();
			float wabs = w.abs();
			CVector Mrem;
			if(wabs < 0.001)
				{Mrem = -w;}
			else
				{Mrem = w * (-cwA*w.abs2() / wabs);}
			Mtot += Mrem;

			//Gravity
			Ftot += CVector(0.0, -g*m, 0.0);

			//Dummy wheel rotation
			//positive angle = driving forward = positive vz
			theCar->m_WheelVelocity = vabs / theCar->m_WheelRadius;
			theCar->m_WheelAngle += dt * theCar->m_WheelVelocity;

			//Dummy roteer-functie:
			float stuur = input->m_Right;
			Mtot += CVector(0,10000*stuur*dt,0);

			//standard acceleration:
			mo->setVelocity(mo->getVelocity() + Ftot * dt/m);

			//standard rotation
			mo->setAngularVelocity(mo->getAngularVelocity() + dt*Mtot*Iinv);
		}
		else
			{printf("Error: object %d is not a car\n", i);}
	}

	return true;
}
