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

#include "player.h"
#include "simulation.h"
#include "physics.h"

int main(int argc, char *argv[])
{
  printf("Welcome to " PACKAGE " version " VERSION ".\n");

  CSimulation *sim = new CPhysics;
  CPlayer *player1 = new CPlayer;
  CPlayer *player2 = new CPlayer;
  CPlayer *player3 = new CPlayer;

  if(!sim->addPlayer(player1))
    {printf("Sim accepteert player1 NIET\n");}

  if(!sim->addPlayer(player2))
    {printf("Sim accepteert player2 NIET\n");}

  if(!sim->addPlayer(player2))
    {printf("Sim accepteert player2 NIET\n");}

  if(!sim->addPlayer(player2))
    {printf("Sim accepteert player2 NIET\n");}

  if(!sim->addPlayer(player3))
    {printf("Sim accepteert player3 NIET\n");}

  printf("Removing player 2\n");
  if(!sim->removePlayer(player2))
    printf("Removing 2 failed\n");

  printf("Removing player 3\n");
  if(!sim->removePlayer(player3))
    printf("Removing 3 failed\n");

  delete sim;

  printf("\nThe players still need to be deleted.\n");
  delete player1;
  delete player2;
  delete player3;

  return EXIT_SUCCESS;
}
