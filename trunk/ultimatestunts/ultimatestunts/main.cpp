/***************************************************************************
                          main.cpp  -  UltimateStunts main
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

//internationalisation:
#include <locale.h>
#include <libintl.h>
#define _(String) gettext (String)

//Standard includes
#include <cstdlib>
#include <cstdio>
#include <vector>
namespace std {}
using namespace std;

#include "cstring.h"
#include "usmisc.h"
#include "lconfig.h"
#include "gamegui.h"

int main(int argc, char *argv[])
{
	printf("Welcome to " PACKAGE " version " VERSION "\n");

	shared_main(argc, argv);

	printf("---Window system\n");
	CGameWinSystem *winsys = new CGameWinSystem("Ultimate Stunts", *theMainConfig);

	printf("---GUI\n");
	CGameGUI *gui = new CGameGUI(*theMainConfig, winsys);

	printf("Starting the GUI\n");
	gui->start();

	printf("\n\nUnloading Ultimate Stunts:\n");

	printf("---GUI\n");
	delete gui;

	printf("---Window system\n");
	delete winsys; //Important; don't remove: this calls SDL_Quit!!!

	printf("\nProgram finished succesfully\n");
	return EXIT_SUCCESS;
}
