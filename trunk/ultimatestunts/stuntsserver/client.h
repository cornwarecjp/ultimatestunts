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

#include "criticalvector.h"

#include "ipnumber.h"
#include "bthread.h"
#include "objectchoice.h"

class CClient
{
public:
	CIPNumber ip;
	unsigned int port;
	vector<int> players;
	vector<int> playerRequests;
	bool ready;
};

class CClientList : public CCriticalVector<CClient>
{
public:
	CClientList()
	{
		maxRequests = 2;
	}

	int addRequest_safe(unsigned int ID, const CObjectChoice &oc)
	{
		enter();
		if(playerRequests.size() >= maxRequests)
		{
			leave();
			return -1;
		}
		
		playerRequests.push_back(oc);
		int ret = playerRequests.size()-1;
		operator[](ID).playerRequests.push_back(ret);
		leave();
		return ret;
	}

	vector<CObjectChoice> playerRequests;
	unsigned int maxRequests;
};

#endif
