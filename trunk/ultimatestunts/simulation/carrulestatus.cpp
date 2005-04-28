/***************************************************************************
                          carrulestatus.cpp  -  Rule status of a racing car
                             -------------------
    begin                : wo apr 6 2005
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

#include <cstdio>

#include "carrulestatus.h"
#include "timer.h"

CCarRuleStatus::CCarRuleStatus()
{
	float t = CTimer().getTime();

	startTime = t;		//time when it started racing
	penaltyTime = 0.0;	//penalty time accumulated while playing
	finishTime = t;		//time when it finished racing

	state = eNotStarted;
}

CCarRuleStatus::~CCarRuleStatus()
{
}

void CCarRuleStatus::start()
{
	float t = CTimer().getTime();

	startTime = t;		//time when it started racing
	penaltyTime = 0.0;	//penalty time accumulated while playing
	finishTime = t;		//time when it finished racing

	state = eRacing;
}

void CCarRuleStatus::finish()
{
	finishTime = CTimer().getTime();	//time when it finished racing
	state = eFinished;

	printf(
		"YOU FINISHED!\n"
		"Driving time: %.2f s\n"
		"Penalty time: %.2f s\n"
		"\n"
		"Total time: %.2f s\n",
		finishTime - startTime,
		penaltyTime,
		finishTime - startTime + penaltyTime
		);
}

void CCarRuleStatus::crash()
{
	finishTime = CTimer().getTime();	//time when it finished racing
	state = eCrashed;
}

void CCarRuleStatus::giveUp()
{
	finishTime = CTimer().getTime();	//time when it finished racing
	state = eGivenUp;
}

void CCarRuleStatus::addPenalty(float time)
{
	printf("PENALTY TIME: %.2f s\n", time);
	penaltyTime += time;	//penalty time accumulated while playing
}

