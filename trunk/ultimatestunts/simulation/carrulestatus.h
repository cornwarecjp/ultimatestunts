/***************************************************************************
                          carrulestatus.h  -  Rule status of a racing car
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

#ifndef CARRULESTATUS_H
#define CARRULESTATUS_H


/**
  *@author CJP
  */

class CCarRuleStatus {
public: 
	CCarRuleStatus();
	~CCarRuleStatus();

	void start();
	void finish();
	void crash();
	void giveUp();
	void addPenalty(float time);

	float startTime;	//time when it started racing
	float penaltyTime;	//penalty time accumulated while playing
	float finishTime;	//time when it finished racing

	enum
	{
		eNotStarted = 0,
		eRacing = 1,
		eFinished = 2,
		eCrashed = 3,
		eGivenUp = 4
	} state;
};

#endif
