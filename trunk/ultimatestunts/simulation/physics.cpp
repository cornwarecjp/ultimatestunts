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

CPhysics::CPhysics(CWorld *w) : CSimulation(w)
{}

CPhysics::~CPhysics(){
}

bool CPhysics::update()
{
	float dt = m_Timer.getdt();
	if(dt > 1.0)
		{printf("Warning: Low update time detected\n"); dt = 1.0;}
	if(dt < 0.0001)
		{printf("Physics debugging message: dt = %f\n", dt);}

//Dummy physics version 2.0 !!!!
#define cwA 10
#define gasmax 10000
#define remmax 10000
#define invstraal 0.05

	for(unsigned int i=0; i < (m_World->m_MovObjs.size()); i++)
	{
		CMovingObject *mo =m_World->m_MovObjs[i];

		mo->updateBodyData();

		if(mo->getType()==CMessageBuffer::car)
		{
			CCar *theCar = (CCar *)mo;
			CCarInput *input = (CCarInput *)mo->m_InputData;

			CMatrix R = mo->getRotationMatrix();

			//Dummy versnel-functie
			float m = mo->m_Mass;
			float gas = input->m_Forward;
			float rem = input->m_Backward;
			CVector v = mo->getVelocity();
			CVector vrel = v / R;
			vrel.x = vrel.y = 0.0;
			float vz = vrel.z; //used for air friction
			float F = -gasmax*gas - copysign(remmax*rem + cwA*vz*vz, vz);
			CVector arel(0,0,F/m); //acceleration

			vrel += (arel * dt); //Now vrel should only have a z-component
			v = vrel * R;
			mo->setVelocity(v);

			//positive angle = driving forward = positive vz
			theCar->m_WheelVelocity = vz / theCar->m_WheelRadius;
			theCar->m_WheelAngle += dt * theCar->m_WheelVelocity;

			//Dummy roteer-functie:
			float stuur = input->m_Right;
			CMatrix dR;
			dR.rotY(-stuur*invstraal*vz*dt);
			R *= dR;
			mo->setRotationMatrix(R);

			//standard acceleration:
			CVector r = mo->getPosition();
			r += v * dt;
			mo->setPosition(r);
		}
		else
			{printf("Error: object %d is not a car\n", i);}
	}

	return true;
}
