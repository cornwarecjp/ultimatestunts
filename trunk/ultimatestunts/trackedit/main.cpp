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

//Graphics stuff
#include "winsystem.h"
#include "terenderer.h"
#include "tecamera.h"

#include "edittrack.h"
#include "temanager.h"

CWinSystem *winsys;
CTERenderer *renderer;
CTECamera *camera;

CTEManager *datamanager;

bool mainloop()
{
	bool ret = true;

	if(winsys->wasPressed('\e'))
		ret = false;

	if(winsys->getKeyState(SDLK_PAGEUP))
		camera->turnUp(0.01);
	if(winsys->getKeyState(SDLK_PAGEDOWN))
		camera->turnUp(-0.01);
	if(winsys->getKeyState(SDLK_LEFT))
		camera->turnRight(-0.01);
	if(winsys->getKeyState(SDLK_RIGHT))
		camera->turnRight(0.01);
	if(winsys->getKeyState(SDLK_UP))
		camera->moveForward(5.0);
	if(winsys->getKeyState(SDLK_DOWN))
		camera->moveForward(-5.0);

	renderer->update();
	return ret;
}

int main(int argc, char *argv[])
{
	printf("Welcome to the " PACKAGE " track editor version " VERSION "\n");

	shared_main(argc, argv);

	printf("\nCreating a window\n");
	winsys = new CWinSystem("Stunts Track Editor", *theMainConfig);

	datamanager = new CTEManager;
	int ID = datamanager->loadObject("tracks/default.track", CParamList(), CDataObject::eTrack);
	if(ID < 0)
	{
		printf("Error: default.track not found\n");
		return 1;
	}
	//CEditTrack *theTrack = (CEditTrack *)(datamanager->getObject(CDataObject::eTrack, ID));

	printf("\nInitialising the rendering engine\n");
	renderer = new CTERenderer(winsys);
	camera = new CTECamera();
	renderer->setCamera(camera);
	renderer->setManager(datamanager);

	printf("\nEntering mainloop\n");
	winsys->runLoop(mainloop, true); //true: swap buffers
	printf("\nLeaving mainloop\n");

	delete renderer;
	delete camera;

	delete datamanager;

	delete winsys; //Important; don't remove: this calls SDL_Quit!!!

	printf("\nProgram finished succesfully\n");

	return EXIT_SUCCESS;
}
