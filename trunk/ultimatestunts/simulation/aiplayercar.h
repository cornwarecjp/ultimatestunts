/***************************************************************************
                          aiplayercar.h  -  description
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

/**
  *@author CJP
  */

class CAIPlayerCar : public CPlayer
{
	public:
		CAIPlayerCar(CWorld *w);
		~CAIPlayerCar();

		virtual bool Update();

	protected:
		bool m_FirstTime;
};

#endif
