/***************************************************************************
                          timer.cpp  -  A timer class
                             -------------------
    begin                : do dec 19 2002
    copyright            : (C) 2002 by CJP
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

//Currently implemented on gettimeofday
//Future versions possibly work with SDL
#include <sys/time.h>

#include "timer.h"

CTimer::CTimer()
{
	m_PreviousTime = getTime();
}
CTimer::~CTimer(){
}

float CTimer::getdt()
{
	float t = getTime();
	float dt = t - m_PreviousTime;
	m_PreviousTime = t;
	return dt;
}

float CTimer::getF()
{
	return (1.0 / getdt());
}

float CTimer::getTime()
{
	//Tijd:
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	//printf("%d;%d\n", tv.tv_sec, tv.tv_usec);
	return (tv.tv_sec & 65535) + tv.tv_usec/1000000.0;
}
