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

/**
  *@author CJP
  */

class CPlayer {
public: 
	CPlayer();
	virtual ~CPlayer();

  virtual void giveWorld(CWorld *w);
protected:
  CWorld *m_World;
};

#endif
