/***************************************************************************
                          client.h  -  Some information about client programs
                             -------------------
    begin                : do jan 13 2005
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

#ifndef CLIENT_H
#define CLIENT_H

#include <vector>
namespace std {}
using namespace std;

#include "ipnumber.h"
#include "bthread.h"
#include "objectchoice.h"

class CClient
{
public:
	CIPNumber ip;
	unsigned int port;
	vector<int> players;
	vector<CObjectChoice> playerRequests;
};

class CClientList : public vector<CClient>, public CBCriticalSection
{
	;
};

#endif
