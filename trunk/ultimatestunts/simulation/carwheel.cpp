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

CCarWheel::CCarWheel()
{
	m_a = 0.0;
	m_w = 0.0;
	m_M = 0.0;

	m_Height = 0.0;
	m_dHeight = 0.0;
	m_Z = CVector(0.0,0.0,1.0);

	m_DesiredSt = 0.0;
	m_SkidVolume = 0.0;

	m_Radius = 0.35;
	m_Iinv_eff = 1.0 / (5.0 * m_Radius * m_Radius); //assume the mass to be 5.0 to 10.0 kg
	m_suspk = 100000.0;
	m_suspd = 10000.0;
	m_tractionStiffness = 0.25;
	m_cornerStiffness = 1.0;
	m_BrakeMax = 100.0;

	m_Mu = 1.0;
	m_Roll = 0.0;
}

CCarWheel::~CCarWheel(){
}

void CCarWheel::load(CLConfig &cfile, const CString &section)
{
	m_Mu = cfile.getValue(section, "mu").toFloat();
	m_Roll = cfile.getValue(section, "roll").toFloat();
	m_suspk = cfile.getValue(section, "suspk").toFloat();
	m_suspd = cfile.getValue(section, "suspd").toFloat();
	m_tractionStiffness = cfile.getValue(section, "tractionstiffness").toFloat();
	m_cornerStiffness = cfile.getValue(section, "cornerstiffness").toFloat();
	m_BrakeMax = cfile.getValue(section, "brakemax").toFloat();
	m_NeutralPos = cfile.getValue(section, "position").toVector();

	//This needs a correct m_Radius setting:
	m_Iinv_eff = 1.0 / (0.5 * cfile.getValue(section, "mass").toFloat() * m_Radius * m_Radius);
}

/*
This function limits the size of F to a maximum value max.
So, it affects the size of F, but not the direction.
Close to the origin, the function is like y=x, so the size
is not affected. As soon as the size of F approaches max,
the curve becomes more horizontal, so that max is never
exceeded. The result is a kind of a "normalized" Pacejka
curve.
Search on the internet about Pacejka curves to get more
information.
*/
void staticlimit(float max, CVector &F, float &skidvolume)
{
	float Fabs = F.abs();

	float skidding = Fabs / max;

	if(skidding > 0.01)
	{
		//Two parts: first a parabola, then a staight line
		float outval = 1.0;
		if(skidding < 2.0)
			outval = 1.0 - 0.25*(skidding-2.0)*(skidding-2.0);

		float mul = outval / skidding;

		/*
		float smoothness = 2.0; //lower value = smoother transition

		float factor = 0.5 + atanf(smoothness*(skidding - 1.0)) / M_PI;

		float mul = 1.0 + factor * (1.0 / skidding - 1.0);
		*/

		if(mul > 1.0) mul = 1.0;
		if(mul < 0.0) mul = 0.0;

		F *= mul;

		//printf("skidding = %.3f\n", skidding);
		//printf("factor   = %.3f\n", factor);
		//printf("mul      = %.3f\n", mul);
		//printf("New F    = %.3f\n", F.abs());

		skidvolume = outval * outval; //factor;
	}
	else
	{
		skidvolume = 0.0;
	}
}

/*
The following function defines the behaviour of the tires.
This is the most important aspect of the physics model of
a racing simulation, so if you want to improve the realism,
then this is probably where you want to be.

parameters:
groundM    : place to put the torque in (same coordinate system as return value)
vlong      : longitudinal velocity of the wheel center. Positive is backward.
vlat       : lateral velocity of the wheel center. Positive is to the right
contactMu  : The static friction limit of the contact surface
contactRoll: The roll friction coefficient of the contact surface

return value:
The tire surface force vector that works on the tire,
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
CVector CCarWheel::getGroundForce(
	CVector &groundM, float vlong, float vlat, float contactMu, float contactRoll)
{
	float vlong_rel = vlong + m_Radius * m_w;

	float speedfactor = fabsf(vlong);
	if(speedfactor < 3.0) speedfactor = 3.0; //avoid weird effects at low speeds
	float klong = -m_tractionStiffness / speedfactor;
	float klat  = -m_cornerStiffness   / speedfactor;

	float slip_long = klong * vlong_rel;
	float slip_lat  = klat  * vlat;

	CVector Slip(slip_lat, 0.0, slip_long);
	staticlimit(contactMu, Slip, m_SkidVolume);
	//fprintf(stderr, "%.f; %.f; %.f; ", 1000*vlong_rel, 1000*slip_long, 1000*Slip.z);

	//Correct skid volume for velocity difference
	// (low relative velocity = no skid sound)
	float v_rel = sqrt(vlong_rel*vlong_rel + vlat*vlat);
	float volume_mult = v_rel / 10.0;
	if(volume_mult > 1.0) volume_mult = 1.0;
	m_SkidVolume *= volume_mult;

	CVector rollFriction(contactRoll * vlong/speedfactor, 0.0, 0.0);
	groundM = m_Fnormal * rollFriction;

	return m_Fnormal * (Slip + CVector(0.0, 1.0, 0.0));
}

/*
The following function defines the behaviour of the brakes.

parameters:
brakeFactor : a value between 0 and 1 that describes how hard we brake

return value:
The torque of the brakes. Should be positive for a forward driving car.

You can use m_w, the angular velocity of the wheel around its axis.
*/
float CCarWheel::getBrakeTorque(float brakeFactor)
{
	/*
	                |  /---------------
	                | /
	                |/
	----------------+-------------------
	               /|
	              / |
	-------------/  |

	The maximum value is m_BrakeMax
	The slope is m_BrakeMax
	The critical speed is 1 rad/s
	*/
	float M = brakeFactor * m_BrakeMax * m_w;
	if(M >  m_BrakeMax) M =  m_BrakeMax;
	if(M < -m_BrakeMax) M = -m_BrakeMax;
	return M;
}
