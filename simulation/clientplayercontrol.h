/***************************************************************************
                          clientplayercontrol.h  -  Remote player management
                             -------------------
    begin                : do sep 11 2003
    copyright            : (C) 2003 by CJP
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

#ifndef CLIENTPLAYERCONTROL_H
#define CLIENTPLAYERCONTROL_H

#include "playercontrol.h"
#include "clientnet.h"

/**
  *@author CJP
  */

class CClientPlayerControl : public CPlayerControl  {
public: 
	CClientPlayerControl(CClientNet *net);
	virtual ~CClientPlayerControl();

	virtual int addPlayer(CObjectChoice choice);
	virtual bool loadObjects();
protected:
	CClientNet *m_Net;
};

#endif
