/***************************************************************************
                          chatsystem.h  -  The "mail delivery office" of Ultimate Stunts
                             -------------------
    begin                : do nov 24 2005
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

#ifndef CHATSYSTEM_H
#define CHATSYSTEM_H

#include <vector> //STL vector template
namespace std {}
using namespace std;

#include "chatmessage.h"
#include "clientnet.h"

/**
  *@author CJP
  */

class CChatSystem {
public: 
	CChatSystem();
	~CChatSystem();

	void sendMessage(CChatMessage &msg); //adds it to the outgoing queue

	void loopBack(); //only for local delivery
	void deliverMessages(); //delivers inqueue to moving objects

	vector<CChatMessage> m_InQueue, m_OutQueue; //public access for network code
};

#endif
