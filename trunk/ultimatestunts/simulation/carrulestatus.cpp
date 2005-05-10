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
#include "cstring.h"

CCarRuleStatus::CCarRuleStatus()
{
	float t = CTimer().getTime();

	startTime = t;		//time when it started racing
	penaltyTime = 0.0;	//penalty time accumulated while playing
	finishTime = t;		//time when it finished racing

	maxTileTime = 0.0;


	state = eNotStarted;
}

CCarRuleStatus::~CCarRuleStatus()
{
}

bool CCarRuleStatus::start()
{
	float t = CTimer().getTime();

	startTime = t;		//time when it started racing
	penaltyTime = 0.0;	//penalty time accumulated while playing
	finishTime = t;		//time when it finished racing

	maxTileTime = 0.0;

	state = eRacing;

	return true;
}

bool CCarRuleStatus::finish()
{
	if(state == eRacing)
	{
		finishTime = CTimer().getTime();	//time when it finished racing
		state = eFinished;

		printf(
			"YOU FINISHED!\n"
			"Driving time: %s\n"
			"Penalty time: %s\n"
			"\n"
			"Total time:   %s\n",
			CString().fromTime(finishTime - startTime).c_str(),
			CString().fromTime(penaltyTime).c_str(),
			CString().fromTime(finishTime - startTime + penaltyTime).c_str()
			);

		return true;
	}

	return false;
}

bool CCarRuleStatus::crash()
{
	if(state == eRacing)
	{
		finishTime = CTimer().getTime();	//time when it finished racing
		state = eCrashed;
		return true;
	}

	return false;
}

bool CCarRuleStatus::giveUp()
{
	if(state != eGivenUp && state != eFinished)
	{
		finishTime = CTimer().getTime();	//time when it finished racing
		state = eGivenUp;
		return true;
	}

	return false;
}

bool CCarRuleStatus::addPenalty(float time)
{
	if(state == eRacing)
	{
		printf("PENALTY TIME: %.2f s\n", time);
		penaltyTime += time;	//penalty time accumulated while playing
		return true;
	}

	return false;
}

