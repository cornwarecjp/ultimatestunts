/***************************************************************************
                          uscore.cpp  -  Game core plus graphics, sound and input
                             -------------------
    begin                : vr okt 15 2004
    copyright            : (C) 2004 by CJP
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

#include <cstdio>

#include <libintl.h>
#define _(String) gettext (String)

#include "uscore.h"
#include "lconfig.h"

#include "car.h"

CUSCore::CUSCore(CGameWinSystem *winsys)
{
	m_WinSys = winsys;

	printf("---Renderer\n");
	m_Renderer = new CGameRenderer(winsys);
	m_Console = new CConsole(winsys);

	printf("---Sound system\n");
	m_SoundSystem = new CSound(*theMainConfig);

	m_NumCameras = 0;
	m_Cameras[0] = NULL;
	m_Cameras[1] = NULL;
	m_Cameras[2] = NULL;
	m_Cameras[3] = NULL;
}

CUSCore::~CUSCore()
{
	printf("---Sound system\n");
	delete m_SoundSystem;
	printf("---Renderer\n");
	delete m_Renderer;
	delete m_Console;
}

bool CUSCore::addCamera(unsigned int objid)
{
	if(m_NumCameras > 3) return false; //max 4 cameras

	CGameCamera *theCam = new CGameCamera;
	theCam->setTrackedObject(objid);
	theCam->m_PrimaryTarget = objid;
	*(m_Cameras + m_NumCameras) = theCam;
	m_NumCameras++;

	return true;
}

void CUSCore::readyAndLoad(LoadStatusCallback callBackFun)
{
	CGameCore::readyAndLoad(callBackFun);

	printf("Setting cameras\n");
	m_Renderer->setCameras(m_Cameras, m_NumCameras);
	m_SoundSystem->setCamera(m_Cameras[0]); //first camera

	m_FPS = 0.0; //to begin with
}

bool CUSCore::update()
{
	bool retval = true;

	//Escape:
	retval = retval && ( !(m_WinSys->globalKeyWasPressed(eExit)) );

	//Cameras:
	for(unsigned int i=0; i < m_NumCameras; i++)
	{
		CGameCamera *theCam = (CGameCamera *)m_Cameras[i];
		if(m_WinSys->playerKeyWasPressed(eCameraChange, i)) theCam->swithCameraMode();
		if(m_WinSys->playerKeyWasPressed(eCameraToggle, i)) theCam->switchTrackedObject();
	}

	//Next song:
	if(m_WinSys->globalKeyWasPressed(eNextSong)) m_SoundSystem->playNextSong();

	//Debug messages
	theWorld->printDebug = m_WinSys->getKeyState('d');

	retval = retval && CGameCore::update();

	for(unsigned int i=0; i < m_NumCameras; i++)
		m_Cameras[i]->update();


	static float topspeed = 0.0;


	//Debugging and text output display:
	m_Console->clear();
	m_Console->print(CString().format( _("Frame rate: %.1f FPS"), 80, m_FPS ));
	m_Console->print(CString().format( _("Top speed in this session: %.1f km/h"), 80,
		(float)(topspeed*3.6) ));
	for(unsigned int i=0; i<m_World->getNumObjects(CDataObject::eMovingObject); i++)
	{
		CMovingObject *mo = m_World->getMovingObject(i);
		if(mo->getType() == CMessageBuffer::car)
		{
			CCar * theCar = (CCar *)mo;
			float vel = theCar->m_Velocity.abs();
			if(vel > topspeed) topspeed = vel;
			float wEngine = theCar->m_Engine.getGearRatio() * theCar->m_Engine.m_MainAxisW;

			m_Console->print(
				CString().format( _("Car %d velocity %.1f km/h; gear %d; %.1f RPM"), 100,
					(int)i, (float)(vel * 3.6), (int)(theCar->m_Engine.m_Gear),
					(float)(60.0 * wEngine / 6.28) ));

			//m_Console->setMessage("test", 2.0);
		}
	}

	//Update the output:
	m_Renderer->update();
	m_Console->draw();
	m_SoundSystem->update();

	return retval;
}

void CUSCore::loadTrackData()
{
	CGameCore::loadTrackData();

	printf("---Graphics track data\n");
	m_Renderer->loadTrackData();
}

void CUSCore::loadMovObjData()
{
	CGameCore::loadMovObjData();

	printf("---Graphics object data\n");
	if(!m_Renderer->loadObjData())
		{printf("Error while loading object graphics\n");}

	printf("---Sound data\n");
	m_SoundSystem->load();
}

void CUSCore::unloadData()
{
	CGameCore::unloadData();

	printf("---Graphics data\n");
	m_Renderer->unloadTrackData();
	m_Renderer->unloadObjData();

	printf("---Sound data\n");
	m_SoundSystem->unload();

	printf("---Cameras\n");
	for(unsigned int i=0; i < m_NumCameras; i++)
		delete *(m_Cameras+i);
	m_NumCameras = 0;
}
