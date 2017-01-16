/***************************************************************************
                          aiplayercar.h  -  An AI player for controlling cars
                             -------------------
    begin                : wo dec 18 2002
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

#ifndef AIPLAYERCAR_H
#define AIPLAYERCAR_H

#include "player.h"
#include "car.h"
#include "carinput.h"

/**
  *@author CJP
  */

class CAIPlayerCar : public CPlayer
{
	public:
		CAIPlayerCar();
		~CAIPlayerCar();

		virtual bool update();

	protected:
		bool m_FirstTime;

		CCar *m_Car;
		CCarInput *m_Carin;

		void setTargetOrientation(CVector nosedir);
		void setTargetSpeed(float v, float &gas, float &brake);

		CVector findNextTargetPos(bool far=false);
};

#endif
