/***************************************************************************
                          playercontrol.h  -  Manage the players
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

#ifndef PLAYERCONTROL_H
#define PLAYERCONTROL_H

#include <vector> //STL vector template
namespace std {}
using namespace std;

#include "world.h"
#include "objectchoice.h"

/**
  *@author CJP
  */

class CPlayerControl {
public: 
	CPlayerControl();
	virtual ~CPlayerControl();

	virtual int addPlayer(CObjectChoice choice);
	virtual bool loadObjects();
	virtual void clearPlayerList();

	const vector<CObjectChoice> &getChoiceList() const
		{return m_LocalChoices;}
protected:
	vector<CObjectChoice> m_LocalChoices;

	//every player's vehicle is a unique object, even if
	//they are loaded from the same file. That's why we
	//have to add a unique parameter for each moving object:
	//the ID. For the rest, the ID is used to identify objects
	//in network messages.
	virtual bool loadOneObject(CObjectChoice &oc, unsigned int ID);
};

#endif
