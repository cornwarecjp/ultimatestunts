/***************************************************************************
                          rulecontrol.h  -  Check if players follow the rules
                             -------------------
    begin                : do sep 11 2003
    copyright            : (C) 2003 by CJP
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

#ifndef RULECONTROL_H
#define RULECONTROL_H

#include <vector> //STL vector template
namespace std {}
using namespace std;

#include "simulation.h"
#include "car.h"
#include "track.h"

#include "vector.h"

/**
  *@author CJP
  */

class CRuleControl : public CSimulation  {
public: 
	CRuleControl();
	~CRuleControl();

	//while playing:
	virtual bool update();
protected:
	bool m_FirstUpdate; //only true on the first iteration
	bool m_BeforeStart; //true as long as t < 0

	bool findStartFinish();

	//Car rules:
	void placeStart();
	void updateCarRules(unsigned int movObjIndex, CCar *car);

	struct SRoutePos
	{
		unsigned int route;
		unsigned int tile;
	};
	float getSmallestDistance(
		const CCarRuleStatus &status,
		SRoutePos &from, SRoutePos to,
		vector<unsigned int> traversedRoutes = vector<unsigned int>());
	float getDirectDistance(SRoutePos from, SRoutePos to);
	void reachRouteRecursive(CCarRuleStatus &status, unsigned int route, unsigned int tile);
	void addPenaltytime(CCar *car, float t);

	void finish(CCar *car);
	bool checkFinished();

	int  m_StartX,  m_StartY,  m_StartH,  m_StartRot,  m_StartIndex,
		m_FinishX, m_FinishY, m_FinishH, m_FinishRot, m_FinishIndex;

	vector<CVector> m_PreviousCarPositions; //needed for accurate finish time checking
};

#endif
