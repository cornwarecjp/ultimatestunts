/***************************************************************************
                          timer.h  -  A timer class
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

#ifndef TIMER_H
#define TIMER_H


/**
  *@author CJP
  */

class CTimer {
public: 
	CTimer();
	~CTimer();

	float getdt();
	float getdt(float min);
	float getdt(float min, float max);
	float getF();

	float getTime() const;
protected:
	float m_PreviousTime;
};

#endif
