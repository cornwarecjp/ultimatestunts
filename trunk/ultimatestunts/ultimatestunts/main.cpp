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

#include <stdlib.h>
#include <stdio.h>

#include "cstring.h"
#include "lconfig.h"

#include "graphicworld.h"
#include "simulation.h"
#include "clientsim.h"
#include "physics.h"

#include "objectchoice.h"

#include "aiplayercar.h"
#include "humanplayer.h"

#include "winsystem.h"
#include "renderer.h"

CGraphicWorld *world;
CPlayer *player1, *player2, *player3, *player4;
CSimulation *sim;

CWinSystem *winsys;
CRenderer *renderer;

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
	player1->Update(); //Makes moving decisions
	player2->Update();
	player3->Update();
	player4->Update();
	retval = retval && sim->Update(); //Modifies world object

	//camera->Update();
	renderer->Update();
	//sound->Update();

	return retval;
}

int main(int argc, char *argv[])
{
	printf("Welcome to " PACKAGE " version " VERSION "\n");

	CLConfig conffile;
	if(!conffile.setFilename("ultimatestunts.conf"))
	{
		printf("Error: could not read ultimatestunts.conf\n"); return 1;
		//TODO: create a default one
	} else {printf("Using ultimatestunts.conf\n");}

	printf("\nCreating a window\n");
	winsys = new CWinSystem(conffile);

	printf("\nCreating (graphic) world object\n");
	world = new CGraphicWorld(conffile);

	while(true) //while we're giving an incorrect answer
	{
		printf(
			"\nThis is a dummy replacement for the GUI.\n"
			"Choose from the following:\n"
			"  1: Play a local game\n"
			"  2: Log in on a remote game\n"
			"  \n"
			"  3: Exit\n"
				);

		int input = 0;
		scanf("%d", &input);
		switch(input)
		{
			case 2:
				printf("Creating client-type simulation\n");
				sim = new CClientSim(world, "localhost", 1500);
				break;
			case 1:
				printf("Creating physics simulation\n");
				sim = new CPhysics(world);
				break;
			case 3:
				return 0;
			default:
				printf("That is not an option\n"); continue;
		}
		break; //The answer was a correct one
	}

	printf("\nLoading the track\n");
	world->loadTrack("tracks/default.track");

	printf("\nInitialising the rendering engine\n");
	renderer = new CRenderer(conffile, world);

	printf("\nCreating 3 AI players\n");
	player1 = new CAIPlayerCar(world);
	player2 = new CAIPlayerCar(world);
	player3 = new CAIPlayerCar(world);

	printf("\nCreating a human player\n");
	player4 = new CHumanPlayer(world);

	printf("\nChoosing cars (or trains, cows, balloons etc)\n");
	CObjectChoice choice;

	printf("\nRegistering players to server\n");
	int id = -1;

	id = sim->addPlayer(choice);
	if(id < 0)
		{printf("Sim doesn't accept player1\n");}
	player1->m_MovingObjectId = id;
	player1->m_PlayerId = 0;

	id = sim->addPlayer(choice);
	if(id < 0)
		{printf("Sim doesn't accept player2\n");}
	player2->m_MovingObjectId = id;
	player2->m_PlayerId = 1;

	id = sim->addPlayer(choice);
	if(id < 0)
		{printf("Sim doesn't accept player3\n");}
	player3->m_MovingObjectId = id;
	player3->m_PlayerId = 2;

	id = sim->addPlayer(choice);
	if(id < 0)
		{printf("Sim doesn't accept player4\n");}
	player4->m_MovingObjectId = id;
	player4->m_PlayerId = 3;

	if(!sim->loadObjects())
		{printf("Error while loading moving objects\n");}

	printf("\nEntering mainloop\n");
	//Creating some white space
	for(int i=1; i<40; i++)
		printf("\n");
	winsys->runLoop(mainloop);
	printf("\nLeaving mainloop\n");

	printf("\nDeleting simulation\n");
	delete sim;

	printf("\nDeleting players\n");
	delete player1;
	delete player2;
	delete player3;
	delete player4;

	printf("\nDeleting world\n");
	delete world;

	printf("\nProgram finished succesfully\n");

	return EXIT_SUCCESS;
}
