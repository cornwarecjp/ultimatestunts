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

#include "lconfig.h"

#include "vector.h"
#include "collisionface.h"

/**
  *@author CJP
  */

class CCarWheel {
public: 
	CCarWheel();
	~CCarWheel();

	//Set m_Radius before calling this function:
	void load(CLConfig &cfile, const CString &section);

	//state variables:
	float m_w; //angular velocity around the axis
	float m_a; //rotation angle around the axis
	float m_DesiredSt; //desired steering angle
	float m_SkidVolume;

	float m_Fnormal; //normal force on the wheel surface

	float m_M; //acumulator for force moment around the axis

	//Wheel behaviour functions (documented in the .cpp file):
	CVector getGroundForce(
		CVector &groundM, float vlong, float vlat, float contactMu, float contactRoll);
	float getBrakeTorque(float brakeFactor);

	//Temorary data
	CVector m_Z; //Z-axis orientation used in car.cpp
	CCollisionFace m_Ground; //the ground plane ( |normal| < 0.5 is no plane)
	float m_Height; //Suspension height position above neutral pos
	float m_dHeight; //Suspension height velocity

	//settings:
	float m_Radius;
	float m_Iinv_eff;
	float m_suspk;
	float m_suspd;
	float m_tractionStiffness, m_cornerStiffness;
	float m_BrakeMax;

	CVector m_NeutralPos; //in car coordinates
	float m_Mu; //static friction limit on asphalt
	float m_Roll; //roll friction coefficient on asphalt
};

#endif
