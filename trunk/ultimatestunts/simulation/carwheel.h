/***************************************************************************
                          carwheel.h  -  A wheel of a car
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

#ifndef CARWHEEL_H
#define CARWHEEL_H

#include "vector.h"

/**
  *@author CJP
  */

class CCarWheel {
public: 
	CCarWheel();
	~CCarWheel();

	//state variables:
	float m_w; //angular velocity around the axis
	float m_a; //rotation angle around the axis
	float m_DesiredSt; //desired steering angle
	float m_SkidVolume;

	float m_Fnormal; //normal force on the wheel surface

	float m_M; //acumulator for force moment around the axis

	//Wheel behaviour functions (documented in the .cpp file):
	CVector getGroundForce(float &groundM, float vlong, float vlat, float contactMu);
	float getBrakeTorque(float brakeFactor);

	//Temorary data
	CVector m_Z;

	//settings:
	float m_Radius;
	float m_Iinv_eff;
	float m_suspk;
	float m_tractionStiffness, m_cornerStiffness;
	float m_BrakeMax;

	CVector m_NeutralPos; //in car coordinates
	float m_Mu; //static friction limit on asphalt
};

#endif
