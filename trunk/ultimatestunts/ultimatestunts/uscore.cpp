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

#include "uscore.h"
#include "lconfig.h"

CUSCore::CUSCore(CGameWinSystem *winsys)
{
	m_WinSys = winsys;

	printf("---Renderer\n");
	m_Renderer = new CGameRenderer(winsys);
	printf("---Sound system\n");
	m_SoundSystem = new CSound(*theMainConfig);

	m_NumCameras = 0;
}

CUSCore::~CUSCore()
{
	printf("---Sound system\n");
	delete m_SoundSystem;
	printf("---Renderer\n");
	delete m_Renderer;

	printf("---Cameras\n");
	for(unsigned int i=0; i < m_NumCameras; i++)
		delete *(m_Cameras+i);
	m_NumCameras = 0;
}

bool CUSCore::addCamera(unsigned int objid)
{
	if(m_NumCameras > 3) return false; //max 4 cameras

	CGameCamera *theCam = new CGameCamera(theWorld);
	theCam->setTrackedObject(objid);
	*(m_Cameras + m_NumCameras) = theCam;
	m_NumCameras++;
	return true;
}

void CUSCore::startGame()
{
	CGameCore::startGame();

	printf("Setting cameras\n");
	m_Renderer->setCameras(m_Cameras, m_NumCameras);
	m_SoundSystem->setCamera(m_Cameras[0]); //first camera
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

	m_Renderer->update();
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
}
