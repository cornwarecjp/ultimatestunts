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
#include "carinput.h"

CPhysics::CPhysics(CWorld *w) : CSimulation(w)
{}

CPhysics::~CPhysics(){
}

bool CPhysics::update()
{
	float dt = m_Timer.getdt();
	if(dt > 1.0)
		{printf("Warning: Low update time detected\n"); dt = 1.0;}
	printf("Now doing physics calculations (dt = %f)\n", dt);

//Dummy physics version 2.0 !!!!
#define m 1000
#define cwA 100
#define gasmax 10000
#define remmax 10000
#define invstraal 0.1

	for(unsigned int i=0; i < (m_World->m_MovObjs.size()); i++)
	{
		CMovingObject *mo =m_World->m_MovObjs[i];

		if(mo->getType()==CMessageBuffer::car)
		{
			CCarInput *input = (CCarInput *)mo->m_InputData;

			//Dummy versnel-functie
			float gas = input->m_Forward;
			float rem = input->m_Backward;
			CVector v = mo->getVelocity();
			float vz = v.z; //used for air friction
			float F = -gasmax*gas - copysign((remmax*rem + cwA*vz*vz),vz);
			CVector a(0,0,F/m); //acceleration. Note that this is only in z-direction

			v += (a * dt); //v is increased by a. So v (if beginning at zero) should only have a z-component
			mo->setVelocity(v);

			//Dummy roteer-functie:
			float stuur = input->m_Right;
			CMatrix dR;
			dR.rotY(-stuur*invstraal*vz*dt);
			CMatrix R = mo->getRotationMatrix();
			R *= dR;
			mo->setRotationMatrix(R);

			//(bijna) standaard versnellen:
			v *= R; //so that v.z points in the looking direction
			CVector r = mo->getPosition();
			r += v * dt;
			mo->setPosition(r);
		}
		else
			{printf("Error: object %d is not a car\n", i);}
	}

	return true;
}
