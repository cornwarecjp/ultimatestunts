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

//Standard includes
#include <stdlib.h>
#include <stdio.h>
#include <vector>

//Frequently used
#include "cstring.h"
#include "lconfig.h"

//Simulation stuff
#include "graphicworld.h"
#include "simulation.h"
#include "clientsim.h"
#include "physics.h"

//Player stuff
#include "objectchoice.h"
#include "aiplayercar.h"
#include "humanplayer.h"

//Graphics stuff
#include "winsystem.h"
#include "renderer.h"
#include "gui.h"
#include "gamecamera.h"

//Sound stuff
#include "sound.h"

CGraphicWorld *world;

vector<CPlayer *> players;

CSimulation *sim;

CWinSystem *winsys;
CRenderer *renderer;
CGUI *gui;
CGameCamera *camera;

CSound *soundsystem;

bool mainloop()
{
 //Debugging 'display'
	printf("\033[1H");
	printf("\033[1G");
	printf("**********\n");
	for(unsigned int i=0; i<world->m_MovObjs.size(); i++)
	{
		CMovingObject *mo = world->m_MovObjs[i];
		CVector r = mo->getPosition();
		CVector v = mo->getVelocity();
		printf("Object %d: position (%.2f,%.2f,%.2f), velocity (%.2f,%.2f,%.2f)\n",
		               i,            r.x, r.y, r.z,       v.x, v.y, v.z);
	}
	printf("**********\n");

	bool retval = true;

	//Escape:
	retval = retval && ( !(bool)(winsys->wasPressed('\e')) );

	//Camera:
	if(winsys->wasPressed('c')) camera->swithCameraMode();

	for(unsigned int i=0; i<players.size(); i++)
		players[i]->update(); //Makes moving decisions

	retval = retval && sim->update(); //Modifies world object

	camera->update();
	renderer->update();
	soundsystem->update();

	return retval;
}

bool addPlayer(CString desc)
{
	CPlayer *p;

	if(desc[0]=='H')
		p = new CHumanPlayer(world, winsys);
	else
		p = new CAIPlayerCar(world);

	CObjectChoice choice;

	int id = sim->addPlayer(choice);
	p->m_MovingObjectId = id;
	p->m_PlayerId = 0;
	if(id < 0)
	{
		printf("Sim doesn't accept player\n");
		delete p;
		return false;
	}

	if(desc[0]=='H') //set the camera to this player:
		camera->setTrackedObject(id);

	players.push_back(p);
	return true;
}

int main(int argc, char *argv[])
{
	printf("Welcome to " PACKAGE " version " VERSION "\n");

	CLConfig conffile(argc, argv);
	if(!conffile.setFilename("ultimatestunts.conf"))
	{
		printf("Error: could not read ultimatestunts.conf\n"); return 1;
		//TODO: create a default one
	} else {printf("Using ultimatestunts.conf\n");}

	printf("\nCreating a window\n");
	winsys = new CWinSystem(conffile);

	world = new CGraphicWorld(conffile);

	printf("\nSetting up the GUI:\n");
	gui = new CGUI(conffile, winsys);

	gui->startFrom("mainmenu");
	while(!( gui->isPassed("mainmenu") )); //waiting for input
	CGUI::eMainMenu maininput =
		(CGUI::eMainMenu)gui->getValue("mainmenu", "choice").toInt();

	switch(maininput)
	{
		case CGUI::LocalGame:
			printf("Creating physics simulation\n");
			sim = new CPhysics(world);
			break;
		case CGUI::JoinNetwork:
		{
			while(!( gui->isPassed("hostmenu") )); //waiting for input
			CString name = gui->getValue("hostmenu", "hostname");
			int port = gui->getValue("hostmenu", "portnumber").toInt();
			printf("Creating client-type simulation with %s:%d\n", name.c_str(), port);
			sim = new CClientSim(world, name, port);
			break;
		}
		default:
		case CGUI::Exit:
			return 0;
	}


	printf("\nLoading the track\n");
	world->loadTrack("tracks/default.track");

	printf("\nInitialising the rendering engine\n");
	renderer = new CRenderer(conffile, world);
	camera = new CGameCamera(world);
	renderer->setCamera(camera);

	printf("\nInitialising sound\n");
	soundsystem = new CSound(conffile, world);
	soundsystem->setCamera(camera);

	while(!( gui->isPassed("playermenu") )); //waiting for input
	int num_players = gui->getValue("playermenu", "number").toInt();
	printf("\nNumber of players to be added: %d\n", num_players);
	for(int i=0; i<num_players; i++)
	{
		CString desc = gui->getValue("playermenu", i);
		printf("%d: \"%s\"\n", i, desc.c_str());
		addPlayer(desc);
	}

	if(!sim->loadObjects())
		{printf("Error while loading moving objects\n");}

	printf("\nEntering mainloop\n");
	//Creating some white space
	for(int i=1; i<40; i++)
		printf("\n");
	winsys->runLoop(mainloop, true); //true: swap buffers
	printf("\nLeaving mainloop\n");

	delete soundsystem;
	delete renderer;
	delete sim;

	for(unsigned int i=0; i<players.size(); i++)
		delete players[i];
	players.clear();

	delete world;

	printf("\nProgram finished succesfully\n");

	return EXIT_SUCCESS;
}
