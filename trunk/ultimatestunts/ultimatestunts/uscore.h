/***************************************************************************
                          uscore.h  -  Game core plus graphics, sound and input
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

#ifndef USCORE_H
#define USCORE_H

#include "gamecore.h"
#include "timer.h"

//Graphics stuff
#include "gamewinsystem.h"
#include "gamerenderer.h"
#include "gamecamera.h"
#include "console.h"

//Sound stuff
#include "sound.h"

/**
  *@author CJP
  */

class CUSCore : public CGameCore  {
public: 
	CUSCore(CGameWinSystem *winsys);
	virtual ~CUSCore();

	bool addCamera(unsigned int objid);

	virtual bool update(); //true = continue false = leave

	virtual void readyAndLoad();

protected:
	CGameWinSystem *m_WinSys;
	CGameRenderer *m_Renderer;
	CConsole *m_Console;

	CCamera *m_Cameras[4]; //max. 4 cameras should be enough
	unsigned int m_NumCameras;

	CSound *m_SoundSystem;

	CTimer m_Timer; //for the FPS counter
	float m_FPS;

	virtual void loadTrackData();
	virtual void loadMovObjData();
	virtual void unloadData();
};

#endif
