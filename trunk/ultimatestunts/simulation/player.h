/***************************************************************************
                          player.h  -  description
                             -------------------
    begin                : Wed Dec 4 2002
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

#ifndef PLAYER_H
#define PLAYER_H

#include "world.h"
#include "message.h"

/**
  *@author CJP
  */

class CPlayer {
public: 
	CPlayer(const CWorld *w);
	virtual ~CPlayer();

	virtual bool Update()=0; //abstract method

	int m_MovingObjectId;
	int m_PlayerId;

protected:
  const CWorld *m_World;
};

#endif
