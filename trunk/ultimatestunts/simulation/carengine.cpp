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
	float factor = exp(-100.0*dt);
	m_MainAxisW = factor * m_MainAxisW + (1.0-factor) * 0.25 * (w1 + w2 + w3 + w4);

	float MEngine = getEngineM(m_MainAxisW * getGearRatio());

	MEngine *= m_Gas;

	m_MainAxisM = MEngine * getGearRatio();
}

float CCarEngine::getWheelM(unsigned int wheelnr)
{
	if(wheelnr == 0 || wheelnr == 1)
		return m_FrontTraction * m_MainAxisM;

	if(wheelnr == 2 || wheelnr == 3)
		return m_RearTraction * m_MainAxisM;

	return 0.0;
}

float CCarEngine::getGearRatio(int gear)
{
	if(gear < 0) gear = m_Gear;
	return m_DifferentialRatio * m_GearRatios[gear];
}

float CCarEngine::getEngineM(float wengine)
{
	/*
	|
	|------------\
	|             \
	|              \
	|               \
	|                \
	|                 \
	L__________________\_______
	|                   \
	|                    \_____
	*/

	float MEngine = m_EngineM; //first part of the curve
	if(wengine > m_MaxEngineW)
	{
		MEngine = m_EngineM - m_dMdw * (wengine - m_MaxEngineW);

		if(MEngine < -0.1 * m_EngineM) MEngine = -0.1*m_EngineM;
	}

	return MEngine;
}
