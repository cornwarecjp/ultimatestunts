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
#include "timer.h"
#include "vector.h"

/**
  *@author CJP
  */

class CRuleControl : public CSimulation  {
public: 
	CRuleControl(CWorld *w);
	~CRuleControl();

	//while playing:
	virtual bool update();
protected:
	bool firstUpdate;

	bool findStartFinish();

	void placeStart();
	bool checkFinished();

	int m_StartX, m_StartY, m_StartH,
		m_FinishX, m_FinishY, m_FinishH;

	CTimer m_Timer;
};

#endif
