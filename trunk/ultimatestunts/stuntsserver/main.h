/***************************************************************************
                          main.h  -  Main objects
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
#ifndef MAIN_H
#define MAIN_H

#include <vector>
namespace std {}
using namespace std;

#include "criticalvector.h"

#include "objectchoice.h"

#include "client.h"
#include "gamecorethread.h"
#include "networkthread.h"
#include "consolethread.h"

//allocated in main.cpp:
extern CCriticalVector<CObjectChoice> ObjectChoices;
extern CGamecoreThread gamecorethread;
extern CNetworkThread networkthread;
extern CConsoleThread consolethread;
extern CClientList Clients;

#endif
