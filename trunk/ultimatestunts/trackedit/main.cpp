/***************************************************************************
                          main.cpp  -  Track-Edit main
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

#include <cstdio>
#include <cstdlib>
#include <cmath>

//Common files
#include "lconfig.h"
#include "cstring.h"
#include "usmisc.h"

#include "trackdocument.h"

//Graphics stuff
#include "winsystem.h"
#include "tegui.h"

int main(int argc, char *argv[])
{
	printf("Welcome to the " PACKAGE " track editor version " VERSION "\n");

	shared_main(argc, argv);

	//Do some settings cheating
	theMainConfig->setValue("animation", "watertesselation", "1");

	printf("\nCreating a window\n");
	CWinSystem *winsys = new CWinSystem("Ultimate Stunts Track Editor", *theMainConfig);

	theTrackDocument = new CTrackDocument;

	CTEGUI *gui = new CTEGUI(*theMainConfig, winsys);

	printf("Starting the GUI\n");
	gui->start();

	delete gui;

	delete theTrackDocument;

	delete winsys; //Important; don't remove: this calls SDL_Quit!!!

	printf("\nProgram finished succesfully\n");

	return EXIT_SUCCESS;
}
