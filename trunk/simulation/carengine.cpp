/***************************************************************************
                          carengine.cpp  -  Engine + drivetrain of a car
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

#include <cmath>

#include "carengine.h"

CCarEngine::CCarEngine()
{
	m_Gear = 1;
	m_MainAxisM = 0.0;
	m_MainAxisW = 0.0;
	m_Gas = 0.0;
}

CCarEngine::~CCarEngine(){
}

void CCarEngine::update(float dt, float w1, float w2, float w3, float w4)
{
	float currentW = 0.5 * m_FrontTraction * (w1 + w2) + 0.5 * m_RearTraction * (w3 + w4);
	float factor = exp(-100.0*dt);
	m_MainAxisW = factor * m_MainAxisW + (1.0-factor) * currentW;

	float MEngine = getEngineM(m_MainAxisW * getGearRatio());

	//fprintf(stderr, "%.f\t%.f\n", m_MainAxisW * getGearRatio(), MEngine);

	MEngine *= m_Gas;

	m_MainAxisM = MEngine * getGearRatio();
}

float CCarEngine::getWheelM(unsigned int wheelnr)
{
	if(wheelnr == 0 || wheelnr == 1)
		return 0.5 * m_FrontTraction * m_MainAxisM;

	if(wheelnr == 2 || wheelnr == 3)
		return 0.5 * m_RearTraction * m_MainAxisM;

	return 0.0;
}

float CCarEngine::getGearRatio(int gear)
{
	if(gear < 0) gear = m_Gear;
	return m_DifferentialRatio * m_GearRatios[gear];
}

float CCarEngine::getEngineM(float wengine)
{
	//Piecewise fit through several points

	float Mengine = 0.0;

	if(wengine < 0.0)
	{
		//negative RPMs
		//TODO: make something more realistic
		Mengine = m_M0;
	}
	else if(wengine < m_w_Mmax)
	{
		float wrel = (wengine-m_w_Mmax)/m_w_Mmax;
		Mengine = m_Mmax - (m_Mmax - m_M0) * wrel*wrel;
	}
	else if(wengine < m_w_Pmax)
	{
		float dw = m_w_Pmax-m_w_Mmax;
		float wrel = (wengine-m_w_Mmax)/dw;
		
		float A = (3 + dw/m_w_Pmax) * m_Pmax / m_w_Pmax - 3*m_Mmax;
		float B = (2 + dw/m_w_Pmax) * m_Pmax / m_w_Pmax - 2*m_Mmax;
		Mengine = m_Mmax + A *wrel*wrel - B*wrel*wrel*wrel;
	}
	else
	{
		float wrel = (wengine - m_w_Pmax) / (m_w_Zero - m_w_Pmax);
		Mengine = (m_Pmax / m_w_Pmax) * (1.0 - wrel*wrel);

		if(Mengine < -0.1 * m_Mmax) Mengine = -0.1*m_Mmax;
	}

	return Mengine;
}
