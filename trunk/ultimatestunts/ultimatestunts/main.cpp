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

#include "world.h"
#include "simulation.h"
#include "physics.h"

#include "objectchoice.h"

#include "aiplayercar.h"
#include "humanplayer.h"

CWorld *world;
CPlayer *player1, *player2, *player3, *player4;
CSimulation *sim;

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
		               i,            r.x, r.y, r.z,       v.z, v.y, v.z);
	}
    	printf("**********\n");

  player1->Update(); //Makes moving decisions
  player2->Update();
  player3->Update();
  player4->Update();
  sim->Update(); //Modifies world object

  //camera->Update();
  //graphics->Update();
  //sound->Update();

  return true; //Infinite loop
}

int main(int argc, char *argv[])
{
	printf("Welcome to " PACKAGE " version " VERSION "\n");

	printf("\nCreating world object\n");
	world = new CWorld;

	printf("\nCreating physics simulation\n");
	sim = new CPhysics(world);

	printf("\nCreating 3 AI players\n");
	player1 = new CAIPlayerCar(world);
	player2 = new CAIPlayerCar(world);
	player3 = new CAIPlayerCar(world);

	printf("\nCreating a human player\n");
	player4 = new CHumanPlayer(world);

	printf("\nChoosing cars (or trains, cows, balloons etc)\n");
	CObjectChoice choice;

	printf("\nRegistering players to server\n");
	if(!sim->addPlayer(player1, choice))
		{printf("Sim doesn't accept player1\n");}
	if(!sim->addPlayer(player2, choice))
		{printf("Sim doesn't accept player2\n");}
	if(!sim->addPlayer(player3, choice))
		{printf("Sim doesn't accept player3\n");}
	if(!sim->addPlayer(player4, choice))
		{printf("Sim doesn't accept player4\n");}

	printf("\nEntering mainloop\n");

	//Creating some white space
	for(int i=1; i<40; i++)
		printf("\n");

	while(mainloop());

	printf("\nLeaving mainloop\n");


	printf("\nRemoving player 2\n");
	if(!sim->removePlayer(player2))
		printf("Removing 2 failed\n");
	printf("\nRemoving player 4\n");
	if(!sim->removePlayer(player4))
		printf("Removing 4 failed\n");

	printf("\nDeleting simulation\n");
	delete sim;

	printf("\nDeleting players\n");
	delete player1;
	delete player2;
	delete player3;
	delete player4;

	printf("\nDeleting world\n");
	delete world;

	return EXIT_SUCCESS;
}
