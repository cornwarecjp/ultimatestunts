/***************************************************************************
                          gamerenderer.h  -  Renderer using world object
                             -------------------
    begin                : di apr 8 2003
    copyright            : (C) 2003 by CJP
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

#ifndef GAMERENDERER_H
#define GAMERENDERER_H

#include "renderer.h"
#include "gamecamera.h"

#include "graphicworld.h"
#include "movingobject.h"

/**
  *@author CJP
  */

class CGameRenderer : public CRenderer  {
public: 
	CGameRenderer(const CWinSystem *winsys);
	virtual ~CGameRenderer();

	bool loadTrackData();
	void unloadTrackData();
	bool loadObjData();
	void unloadObjData();

	//DON't use setCamera with CGameRenderer, use this instead!
	void setCameras(CGameCamera **cams, unsigned int num);

	virtual void update();

	CGraphicWorld *m_GraphicWorld; //TODO: place in CUSCore

protected:
	CGameCamera **m_Cameras;
	unsigned int m_NumCameras;
	unsigned int m_CurrentCamera;
	int camx, camy, camz;


	void clearScreen();
	void updateShadows();
	void updateReflections();
	void selectCamera(unsigned int n, bool threed = true);
	void renderScene();

	void viewBackground();
	void viewMovObj(unsigned int n);

	void viewDashboard(unsigned int n);
	void viewLensFlare();

	void viewTrack_normal();
	void viewTrack_displaylist();

	void viewTrackPart(
		int xmin,int ymin,
		int xmax,int ymax,
		int dx,  int dy,
		int cur_zpos);
	void viewPilaar(int x, int y, int cur_zpos);

	int m_UpdateBodyReflection;

	int m_ViewportW, m_ViewportH; //pixel size of the current viewport
};

#endif
