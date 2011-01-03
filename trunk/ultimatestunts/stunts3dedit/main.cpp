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

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "pi.h"

//Common files
#include "lconfig.h"
#include "cstring.h"
#include "usmisc.h"

//Graphics stuff
#include "winsystem.h"
#include "editrenderer.h"
#include "editcamera.h"

#include "editgraphobj.h"
#include "textureloader.h"

//Key configuration:
enum eKeyconf {
	eExit = '\e',
	eLoad = 'F',
	eSave = 'f',
	eSettings = 'S',

	ePrimitive = 'p',
	eVertex = 'v',

	eNew = 'n',
	eChange = 'c',
	eChangePrimitive = 'C',
	eDuplicatePrimitive = 'd',
	eSetCollision = 'l',

	eRotate = 'r',
	eScale = 's',
	eMirror = 'm',
	eClamp = 'a',
	eSplit = 'P',
	eOrder = 'o',
	eGenerate = 'g',
	eTranslate = 't',

	eRotateTexture = 'R',
	eTranslateTexture = 'T',

	eHelp = 'h'
};

CWinSystem *winsys;
CEditRenderer *renderer;
CEditCamera *camera;

CEditGraphObj *graphobj;
CTextureLoader *texloader;
CString VisibleLODs;

//State variables:
int curr_primitive = 0, curr_vertex = 0;

CString getInput(CString question="")
{
	printf("%s", question.c_str());
	CString out;
	char input[80];
	while(true)
	{
		if(scanf("%s", input) != 1)
		{
			printf("Error reading input\n");
		}

		out += input;
		char c = getchar();
		if(c == '\n') break;
		out += c;
	}
	return out;
}

//Now the functions:
#include "generalfuncs.cpp"
#include "transformfuncs.cpp"
#include "toolfuncs.cpp"
#include "editfuncs.cpp"

bool mainloop()
{
	bool ret = true;

	if(winsys->wasPressed(eExit))
		ret = false;

	if(winsys->wasPressed(eLoad))
		loadFunc();
	if(winsys->wasPressed(eSave))
		saveFunc();
	if(winsys->wasPressed(eSettings))
		settingsFunc();
	if(winsys->wasPressed(ePrimitive))
		primitiveFunc();
	if(winsys->wasPressed(eVertex))
		vertexFunc();
	if(winsys->wasPressed(eNew))
		newFunc();
	if(winsys->wasPressed(SDLK_DELETE))
		deleteFunc();
	if(winsys->wasPressed(eChange))
		changeFunc();
	if(winsys->wasPressed(eChangePrimitive))
		changePrimitiveFunc();
	if(winsys->wasPressed(eDuplicatePrimitive))
		duplicatePrimitiveFunc();
	if(winsys->wasPressed(eSetCollision))
		setCollisionFunc();
	if(winsys->wasPressed(eRotate))
		rotateFunc();
	if(winsys->wasPressed(eScale))
		scaleFunc();
	if(winsys->wasPressed(eMirror))
		mirrorFunc();
	if(winsys->wasPressed(eClamp))
		clampFunc();
	if(winsys->wasPressed(eSplit))
		splitFunc();
	if(winsys->wasPressed(eOrder))
		orderFunc();
	if(winsys->wasPressed(eGenerate))
		generateFunc();
	if(winsys->wasPressed(eTranslate))
		translateFunc();
	if(winsys->wasPressed(eRotateTexture))
		rotateTextureFunc();
	if(winsys->wasPressed(eTranslateTexture))
		translateTextureFunc();
	if(winsys->wasPressed(eHelp))
		helpFunc();

	if(winsys->wasPressed(SDLK_END))
		camera->flipCameraCenter();

	if(winsys->getKeyState(SDLK_PAGEUP))
		camera->incrDist(-0.1);
	if(winsys->getKeyState(SDLK_PAGEDOWN))
		camera->incrDist(0.1);
	if(winsys->getKeyState(SDLK_LEFT))
		camera->incrXAngle(0.01);
	if(winsys->getKeyState(SDLK_RIGHT))
		camera->incrXAngle(-0.01);
	if(winsys->getKeyState(SDLK_UP))
		camera->incrYAngle(0.01);
	if(winsys->getKeyState(SDLK_DOWN))
		camera->incrYAngle(-0.01);

	renderer->update();
	return ret;
}

int main(int argc, char *argv[])
{
	printf("Welcome to the " PACKAGE " 3D editor version " VERSION "\n");

	shared_main(argc, argv);
	
	VisibleLODs = "1234c";

	printf("\nCreating a window\n");
	winsys = new CWinSystem("Stunts 3D Edit");


	printf("\nLoading textures from textures.dat\n");
	texloader = new CTextureLoader("textures.dat");

	graphobj = new CEditGraphObj;
	graphobj->m_MatArray = texloader->m_TexArray;
	graphobj->render(VisibleLODs);

	printf("\nInitialising the rendering engine\n");
	renderer = new CEditRenderer;
	camera = new CEditCamera;
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
