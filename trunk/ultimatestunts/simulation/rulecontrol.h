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

#include "simulation.h"
#include "car.h"

#include "timer.h"
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
	bool firstUpdate;

	bool findStartFinish();

	//Car rules:
	void placeStart();
	void updateCarRules(CCar *car);
	bool checkFinished();

	int m_StartX, m_StartY, m_StartH, m_StartIndex,
		m_FinishX, m_FinishY, m_FinishH, m_FinishIndex;

	CTimer m_Timer;
};

#endif
