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
#include <cstdlib>
#include <cstdio>
#include <vector>
namespace std {}
using namespace std;

//Frequently used
#include "cstring.h"
#include "lconfig.h"
#include "filecontrol.h"
#include "usmacros.h"

//Simulation stuff
#include "playercontrol.h"
#include "clientplayercontrol.h"
#include "clientsim.h"
#include "rulecontrol.h"
#include "physics.h"
#include "world.h"
#include "car.h"

#include "usserver.h"

//Player stuff
#include "objectchoice.h"
#include "aiplayercar.h"
#include "humanplayer.h"

//Graphics stuff
#include "gamewinsystem.h"
#include "gamerenderer.h"
#include "gui.h"
#include "gamecamera.h"

//Sound stuff
#include "sound.h"

CWorld *world = NULL;

vector<CPlayer *> players;

CPlayerControl *pctrl;
vector<CSimulation *> simulations;
CClientNet *clientnet = NULL;
CUSServer *server = NULL;

CGameWinSystem *winsys = NULL;
CGameRenderer *renderer = NULL;
CGUI *gui = NULL;

CCamera *cameras[16]; //max. 16 cameras should be enough
unsigned int numCameras = 0;

CSound *soundsystem = NULL;

float topspeed = 0.0;

bool mainloop()
{
#ifdef DEBUGMSG
 //Debugging 'display'
	printf("\033[1H");
	printf("\033[1G");
	printf("**********\n");
	for(unsigned int i=0; i<world->m_MovObjs.size(); i++)
	{
		CMovingObject *mo = world->m_MovObjs[i];
		if(mo->getType() == CMessageBuffer::car)
		{
			CCar * theCar = (CCar *)mo;
			float vel = theCar->m_Bodies[0].getVelocity().abs();
			if(vel > topspeed) topspeed = vel;
			float wEngine = theCar->getGearRatio() * theCar->m_MainAxisVelocity;
			printf("Top speed in this session: %.2f km/h\n", topspeed * 3.6);
			printf("Car %d: velocity %.2f km/h; gear %d; %.2f RPM\n"
					"wheel %.2f rad/s; axis %.2f rad/s; engine %.2f rad/s\n",
						i,          vel * 3.6, theCar->m_Gear, 60.0 * wEngine / 6.28,
						vel / 0.35, theCar->m_MainAxisVelocity, wEngine);
		}
	}
	printf("**********\n");
#endif

	bool retval = true;

	//Escape:
	retval = retval && ( !(winsys->globalKeyWasPressed(eExit)) );

	//Cameras:
	for(unsigned int i=0; i < numCameras; i++)
	{
		CGameCamera *theCam = (CGameCamera *)cameras[i];
		if(winsys->playerKeyWasPressed(eCameraChange, i)) theCam->swithCameraMode();
		if(winsys->playerKeyWasPressed(eCameraToggle, i)) theCam->switchTrackedObject();
	}

	//Next song:
	if(winsys->globalKeyWasPressed(eNextSong)) soundsystem->playNextSong();

	//Debug messages
	world->printDebug = winsys->getKeyState('d');

	for(unsigned int i=0; i<players.size(); i++)
		players[i]->update(); //Makes moving decisions

	for(unsigned int i=0; i<simulations.size(); i++)
		retval = retval && simulations[i]->update(); //Modifies world object

	for(unsigned int i=0; i < numCameras; i++)
		cameras[i]->update();

	renderer->update();
	soundsystem->update();

	return retval;
}

bool addPlayer(bool isHuman, CString name, CObjectChoice choice)
{
	CPlayer *p;

	if(isHuman)
		p = new CHumanPlayer(winsys, numCameras);
	else
		p = new CAIPlayerCar();

	int id = pctrl->addPlayer(choice);
	p->m_MovingObjectId = id;
	p->m_PlayerId = 0;
	if(id < 0)
	{
		printf("Sim doesn't accept player\n");
		delete p;
		return false;
	}

	if(isHuman) //set the camera to this player:
	{
		CGameCamera *theCam = new CGameCamera(world);
		theCam->setTrackedObject(id);
		*(cameras + numCameras) = theCam;
		numCameras++;
	}

	players.push_back(p);
	return true;
}

void start(int argc, char *argv[]) //first things before becoming interactive
{
	printf("Welcome to " PACKAGE " version " VERSION "\n");

	theMainConfig = new CLConfig(argc, argv);
	if(!theMainConfig->setFilename("ultimatestunts.conf"))
	{
		printf("Error: could not read ultimatestunts.conf\n");
		//TODO: create a default one
	} else {printf("Using ultimatestunts.conf\n");}

	CFileControl *fctl = new CFileControl;
	{
		//Default:
		CString DataDir = ""; //try in default directories, like "./"

		CString cnf = theMainConfig->getValue("files", "datadir");
		if(cnf != "")
		{
			if(cnf[cnf.length()-1] != '/') cnf += '/';
			DataDir = cnf;
		}
		fctl->setDataDir(DataDir);
	}

	printf("Initialising Ultimate Stunts:\n---Window system\n");
	winsys = new CGameWinSystem("Ultimate Stunts", *theMainConfig);

	world = new CWorld();

	printf("---GUI\n");
	gui = new CGUI(*theMainConfig, winsys);

	printf("---Sound system\n");
	soundsystem = new CSound(*theMainConfig);

	printf("---Rendering engine\n");
	renderer = new CGameRenderer(winsys);
}

void end() //Last things before exiting
{
	printf("Unloading Ultimate Stunts:\n---Sound system\n");
	if(soundsystem!=NULL)
		delete soundsystem;
	printf("---Rendering engine\n");
	if(renderer!=NULL)
		delete renderer;
	printf("---Cameras\n");
	if(cameras != NULL)
	{
		for(unsigned int i=0; i < numCameras; i++)
			delete *(cameras+i);
	}
	printf("---Simulations\n");
	for(unsigned int i=0; i<simulations.size(); i++)
		delete simulations[i];
	simulations.clear();

	printf("---Players\n");
	for(unsigned int i=0; i<players.size(); i++)
		delete players[i];
	players.clear();

	printf("---Player control center\n");
	if(pctrl!=NULL)
		delete pctrl;

	printf("---Network\n");
	if(clientnet!=NULL)
		delete clientnet;

	printf("---World data\n");
	if(world!=NULL)
		delete world;

	printf("---Server\n");
	if(server!=NULL)
		delete server; //also stops the server process (if existing)

	printf("---GUI\n");
	if(gui!=NULL)
		delete gui;

	printf("---Window system\n");
	if(winsys!=NULL)
		delete winsys; //Important; don't remove: this calls SDL_Quit!!!

	printf("\nProgram finished succesfully\n");
}

int main(int argc, char *argv[])
{
	start(argc, argv);

	//The track filename:
	CString trackfile = "tracks/default.track";

	//MENU SECTION:
	gui->startFrom("mainmenu");
	while(!( gui->isPassed("mainmenu") )); //waiting for input
	CGUI::eMainMenu maininput =
		(CGUI::eMainMenu)gui->getValue("mainmenu").toInt();

	switch(maininput)
	{
		case CGUI::LocalGame:
			pctrl = new CPlayerControl;
			simulations.push_back(new CRuleControl(world));
			simulations.push_back(new CPhysics(theMainConfig, world));
			while(!( gui->isPassed("trackmenu") )); //waiting for input
			trackfile = gui->getValue("trackmenu");
			break;
		case CGUI::NewNetwork:
		{
			//Server menu:
			while(!( gui->isPassed("servermenu") )); //waiting for input
			CString name = "localhost";
			int port = gui->getValue("servermenu", "portnumber").toInt();

			//Track menu:
			while(!( gui->isPassed("trackmenu") )); //waiting for input
			trackfile = gui->getValue("trackmenu");

			server = new CUSServer(port, trackfile);
			printf("Creating client-type simulation with %s:%d\n", name.c_str(), port);
			clientnet = new CClientNet(name, port);
			pctrl = new CClientPlayerControl(clientnet);
			simulations.push_back(new CClientSim(clientnet, world));
			simulations.push_back(new CPhysics(theMainConfig, world));
			break;
		}
		case CGUI::JoinNetwork:
		{
			while(!( gui->isPassed("hostmenu") )); //waiting for input
			CString name = gui->getValue("hostmenu", "hostname");
			int port = gui->getValue("hostmenu", "portnumber").toInt();
			printf("Creating client-type simulation with %s:%d\n", name.c_str(), port);
			clientnet = new CClientNet(name, port);
			pctrl = new CClientPlayerControl(clientnet);
			CClientSim *csim = new CClientSim(clientnet, world);
			trackfile = csim->getTrackname();
			simulations.push_back(csim);
			simulations.push_back(new CPhysics(theMainConfig, world));
			break;
		}
		default:
		case CGUI::Exit:
			end();
			return EXIT_SUCCESS;
	}

	printf("\nLoading the track\n");
	world->loadTrack(trackfile);
	renderer->loadTrackData();

	//Set up the players
	while(!( gui->isPassed("playermenu") )); //waiting for input
	int num_players = gui->getValue("playermenu", "number").toInt();
	printf("\nNumber of players to be added: %d\n", num_players);
	for(int i=0; i<num_players; i++)
	{
		CString name = gui->getValue("playermenu", CString("name ") + i);
		bool isHuman = gui->getValue("playermenu", CString("ishuman ") + i) == "true";
		CString carFile = gui->getValue("playermenu", CString("carfile ") + i);
		CObjectChoice oc;
		oc.m_Filename = carFile;
		printf("%d: \"%s\" driving %s\n", i, name.c_str(), carFile.c_str());
		addPlayer(isHuman, name, oc);
	}

	//Now the players are set, we can give the cameras to the renderer and sound system:
	renderer->setCameras(cameras, numCameras);
	soundsystem->setCamera(cameras[0]); //first camera

	printf("\nLoading moving objects\n");
	if(!pctrl->loadObjects())
		{printf("Error while loading moving objects\n");}
	if(!renderer->loadObjData())
		{printf("Error while loading object graphics\n");}


	printf("\nLoading the sound samples\n");
	soundsystem->load();

	printf("\nEntering mainloop\n");
	//Creating some white space
	for(int i=1; i<40; i++)
		printf("\n");
	winsys->runLoop(mainloop, true); //true: swap buffers
	printf("\nLeaving mainloop\n");

	end();
	return EXIT_SUCCESS;
}
