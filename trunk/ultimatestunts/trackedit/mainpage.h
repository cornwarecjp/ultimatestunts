/***************************************************************************
                          mainpage.h  -  The main page of the trackedit interface
                             -------------------
    begin                : do dec 12 2006
    copyright            : (C) 2006 by CJP
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

#ifndef MAINPAGE_H
#define MAINPAGE_H

#include "guipage.h"

#include "temanager.h"

#include "terenderer.h"
#include "tecamera.h"
#include "edittrack.h"

/**
  *@author CJP
  */

class CMainPage : public CGUIPage  {
public: 
	CMainPage();
	~CMainPage();

	virtual int onKeyPress(int key);
	virtual int onMouseClick(int x, int y, unsigned int buttons);
	virtual int onMouseMove(int x, int y, unsigned int buttons);
	virtual int onIdle();

	void resetCameraPosition();

	unsigned int getMenuSelection();

protected:
	CTERenderer *m_Renderer;
	CTECamera *m_Camera;

	CEditTrack *m_TrackCache;

	int m_PrevMouseX, m_PrevMouseY;
};

#endif
