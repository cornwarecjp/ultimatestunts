/***************************************************************************
                          main.cpp  -  3D-Edit main
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

#include <stdio.h>
#include <stdlib.h>

//Common files
#include "lconfig.h"
#include "cstring.h"

//Graphics stuff
#include "winsystem.h"
#include "editrenderer.h"
#include "editcamera.h"

#include "editgraphobj.h"
#include "textureloader.h"

CWinSystem *winsys;
CEditRenderer *renderer;
CEditCamera *camera;

CEditGraphObj *graphobj;
CTextureLoader *texloader;

CString topdir;

CString getInput()
{
	char input[80];
	scanf("%s", input);
	return input;
}

bool mainloop()
{
	bool ret = true;

	const Uint8 *keystate = winsys->getKeyState();

	if(winsys->wasPressed('\e'))
		ret = false;

	if(keystate[SDLK_PAGEUP])
		camera->incrDist(-1.0);
	if(keystate[SDLK_PAGEDOWN])
		camera->incrDist(1.0);
	if(keystate[SDLK_LEFT])
		camera->incrXAngle(0.1);
	if(keystate[SDLK_RIGHT])
		camera->incrXAngle(-0.1);
	if(keystate[SDLK_UP])
		camera->incrYAngle(0.1);
	if(keystate[SDLK_DOWN])
		camera->incrYAngle(-0.1);

	renderer->update();
	return ret;
}

int main(int argc, char *argv[])
{
	CLConfig conffile(argc, argv);
	if(!conffile.setFilename("ultimatestunts.conf"))
	{
		printf("Error: could not read ultimatestunts.conf\n"); return 1;
		//TODO: create a default one
	} else {printf("Using ultimatestunts.conf\n");}

	printf("\nCreating a window\n");
	winsys = new CWinSystem(conffile);

	topdir = conffile.getValue("files", "datadir");
	if(topdir != "" && topdir[topdir.length()-1] != '/')
		topdir += '/';
	printf("Filenames are relative to \"%s\"\n", topdir.c_str());

	printf("\nLoading textures from textures.dat\n");
	texloader = new CTextureLoader(conffile, "textures.dat");

	printf("Please enter the filename: ");
	CString fn = getInput();
	graphobj = new CEditGraphObj;
	printf("Loading graphic object...\n");
	graphobj->loadFromFile(topdir + fn, texloader->m_TexArray);
	printf("...done\n");

	printf("\nInitialising the rendering engine\n");
	renderer = new CEditRenderer(conffile);
	camera = new CEditCamera();
	renderer->setCamera(camera);
	renderer->setGraphobj(graphobj);

	printf("\nEntering mainloop\n");
	winsys->runLoop(mainloop, true); //true: swap buffers
	printf("\nLeaving mainloop\n");

	delete renderer;
	delete winsys; //Important; don't remove: this calls SDL_Quit!!!

	printf("\nProgram finished succesfully\n");

  return EXIT_SUCCESS;
}
