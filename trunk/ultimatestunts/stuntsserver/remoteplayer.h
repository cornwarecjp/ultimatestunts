/***************************************************************************
                          remoteplayer.h  -  A player that gets input from a network
                             -------------------
    begin                : ma jan 17 2005
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

#ifndef REMOTEPLAYER_H
#define REMOTEPLAYER_H

#include "player.h"

/**
  *@author CJP
  */

class CRemotePlayer : public CPlayer  {
public: 
	CRemotePlayer();
	virtual ~CRemotePlayer();

	virtual bool update();
};

#endif
