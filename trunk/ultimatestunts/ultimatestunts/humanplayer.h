/***************************************************************************
                          humanplayer.h  -  A player-class getting input data from the real world ;-)
                             -------------------
    begin                : Thu Dec 5 2002
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

#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "player.h"

/**
  *@author CJP
  */

class CHumanPlayer : public CPlayer  {
public: 
	CHumanPlayer(const CWorld *w);
	~CHumanPlayer();

	virtual bool Update();
};

#endif
