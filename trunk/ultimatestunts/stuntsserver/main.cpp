/***************************************************************************
                          main.cpp  -  Stunts-Server main
                             -------------------
    begin                : wo nov 20 23:23:28 CET 2002
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <cstdio>

#include <vector>
namespace std {}
using namespace std;

#undef USE_SDL
#ifdef HAVE_SDL
#define USE_SDL
#endif

#ifdef USE_SDL
#include "SDL.h"
#endif

#include "lconfig.h"
#include "usmisc.h"

#include "main.h"

#include "objectchoice.h"
#include "chatmessage.h"

#include "client.h"
#include "gamecorethread.h"
#include "networkthread.h"
#include "consolethread.h"

//The game objects from main.h:
CCriticalVector<CObjectChoice> ObjectChoices;
CGamecoreThread gamecorethread;
CNetworkThread networkthread;
CConsoleThread consolethread;
CClientList Clients;

int main(int argc, char *argv[])
{
	printf("Welcome to the " PACKAGE " server version " VERSION "\n");

	shared_main(argc, argv);

#ifdef USE_SDL
	SDL_Init(SDL_INIT_TIMER);
#endif

	printf("Parsing input arguments\n");
	for(int i=0; i<argc; i++)
	{
		CString arg = argv[i];
		int is = arg.inStr('=');
		if(is > 0)
			consolethread.cmd_set(arg);
	}

	printf("\n---Gamecore\n");
	gamecorethread.m_GameCore = new CGameCore;

	printf("Starting network thread\n");
	networkthread.start();

	consolethread.startConsole();

	printf("\n---Gamecore\n");
	delete gamecorethread.m_GameCore;

	printf("Stopping network thread\n");
	if(!networkthread.stop())
		printf("For some reason, stopping the thread failed\n");

	return EXIT_SUCCESS;
}
