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

#include <stdlib.h>
#include <stdio.h>

#include "simulation.h"
#include "serversim.h"
#include "clientsim.h"
#include "physics.h"

#include "world.h"

#include "objectchoice.h"
#include "aiplayercar.h"

#include "lconfig.h"

CWorld *world;
CPlayer *player1, *player2, *player3;
CSimulation *sim, *subsim1;

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

  player1->Update(); //Makes moving decisions
  player2->Update();
  player3->Update();

  sim->Update(); //Modifies world object

  return true; //Infinite loop
}

int main(int argc, char *argv[])
{
	char inpbuffer[80];
	printf("Welcome to the " PACKAGE " server version " VERSION "\n");

	CLConfig conffile;
	conffile.setFilename("ultimatestunts.conf");

	printf("\nCreating world object\n");
	world = new CWorld(conffile);

	world->loadTrack("tracks/default.track");

	printf("\nDo you want to use the server to act as a client? (y/n)");
	scanf("%s", inpbuffer);
	if(inpbuffer[0]=='y' || inpbuffer[0]=='Y')
	{
		printf("Creating client-type simulation\n");
		subsim1 = new CClientSim(world, "localhost", 1500);
	}
	else
	{
		printf("Creating physics simulation\n");
		subsim1 = new CPhysics(world);
	}
	printf("\nCreating server simulation\n");
	{
		CServerSim *ssim = new CServerSim(world, 1500);
		ssim->addSubSim(subsim1);
		sim = (CSimulation *)ssim;
	}

	printf("\nCreating 3 AI players\n");
	player1 = new CAIPlayerCar(world);
	player2 = new CAIPlayerCar(world);
	player3 = new CAIPlayerCar(world);

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

	if(!sim->loadObjects())
		printf("Error while loading moving objects\n");

	printf("\nEntering mainloop\n");

	//Creating some white space
	for(int i=1; i<40; i++)
		printf("\n");

	while(mainloop());

	printf("\nLeaving mainloop\n");

	printf("\nDeleting simulation\n");
	delete sim;

	printf("\nDeleting players\n");
	delete player1;
	delete player2;
	delete player3;

	printf("\nDeleting world\n");
	delete world;

  return EXIT_SUCCESS;
}
