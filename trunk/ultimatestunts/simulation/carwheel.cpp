/***************************************************************************
                          carwheel.cpp  -  A wheel of a car
                             -------------------
    begin                : di mrt 8 2005
    copyright            : (C) 2005 by CJP
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

#include "carwheel.h"

#include <cstdio>
#include <cmath>

#ifndef M_PI
#define M_PI 3.1415926536
#endif
#define DBLPI (2.0*M_PI)

CCarWheel::CCarWheel()
{
	m_a = 0.0;
	m_w = 0.0;
	m_M = 0.0;
	m_Z = CVector(0.0,0.0,1.0);
	m_DesiredSt = 0.0;
	m_SkidVolume = 0.0;

	m_Radius = 0.35;
	m_Iinv_eff = 1.0 / (5.0 * m_Radius * m_Radius); //assume the mass to be 5.0 to 10.0 kg
}

CCarWheel::~CCarWheel(){
}

void staticlimit(float max, CVector &F, float &skidvolume)
{
	float Fabs = F.abs();

	float skidding = Fabs / max;

	if(skidding > 0.01)
	{
		float smoothness = 2.0; //lower value = smoother transition

		float factor = 0.5 + atanf(smoothness*(skidding - 1.0)) / M_PI;

		float mul = 1.0 + factor * (1.0 / skidding - 1.0);

		if(mul > 1.0) mul = 1.0;
		if(mul < 0.0) mul = 0.0;

		//printf("skidding = %.3f\n", skidding);
		//printf("factor   = %.3f\n", factor);
		//printf("mul      = %.3f\n", mul);

		F *= mul;

		skidvolume = factor;
	}
}

/*
The following function defines the behaviour of the tyres.
This is the most important aspect of the physics model of
a racing simulation, so if you want to improve the realism,
then this is probably where you want to be.

parameters:
groundM  : place to put the torque in
vlong    : longitudinal velocity of the wheel center. Positive is backward.
vlat     : lateral velocity of the wheel center. Positive is to the right
contactMu: The static friction limit of the contact surface

return value:
The tyre surface force vector that works on the tyre,
with the following axis directions:
x = to the right
y = to the top
z = to the back

The following members are also extremely useful:
m_Radius  : the radius of the wheel
m_Iinv_eff: the inverse of the axial moment of inertial (including the engine etc.)
m_Fnormal : the normal force that works on the tyre surface
m_w       : the angular velocity of the wheel around its axis

You can also set m_SkidVolume to a value between 0.0 and 1.0

All units are SI units, like everywhere in Ultimate Stunts. so:
forces             are in Newtons (N)
sizes              are in meters (m)
masses             are in kilograms (kg)
velocities         are in meters per second (m/s)
angular velocities are in rads per second (rad/s)

*/
CVector CCarWheel::getGroundForce(float &groundM, float vlong, float vlat, float contactMu)
{
	float vlong_rel = vlong + m_Radius * m_w;

	float Flong_norm = -m_tractionStiffness * vlong_rel;
	float Flat_norm = -m_cornerStiffness * vlat;

	CVector Fhor(Flat_norm, 0.0, Flong_norm);
	staticlimit(contactMu, Fhor, m_SkidVolume);

	return m_Fnormal * (Fhor + CVector(0.0, 1.0, 0.0));
}
