/***************************************************************************
                          carengine.h  -  Engine + drivetrain of a car
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

#ifndef CARENGINE_H
#define CARENGINE_H

#include <vector> //STL vector template
namespace std {}
using namespace std;

/**
  *@author CJP
  */

class CCarEngine {
public: 
	CCarEngine();
	~CCarEngine();

	//settings:
	vector<float> m_GearRatios;
	float m_DifferentialRatio;
	float m_EngineM, m_MaxEngineW, m_dMdw;

	float m_FrontTraction, m_RearTraction;

	//input:
	float m_Gas;
	unsigned int m_Gear;

	//from wheels:
	float m_MainAxisW;
	void update(float dt, float w1, float w2, float w3, float w4);

	//to wheels:
	float m_MainAxisM;
	float getWheelM(unsigned int wheelnr);

	float getGearRatio(int gear = -1);
	float getEngineM(float wengine);
};

#endif
