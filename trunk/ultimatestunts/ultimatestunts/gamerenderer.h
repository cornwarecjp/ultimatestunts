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
	void setCameras(CCamera **cams, unsigned int num);

	virtual void update();

protected:
	CCamera **m_Cameras;
	unsigned int m_NumCameras;
	unsigned int m_CurrentCamera;
	int camx, camy, camz;


	void clearScreen();
	void updateReflections();
	void selectCamera(unsigned int n);
	void renderScene();

	void viewBackground();
	void viewMovObj(unsigned int n);
	void viewPilaar(int x, int y, int cur_zpos);

	void viewTrackPart(
		int xmin,int ymin,
		int xmax,int ymax,
		int dx,  int dy,
		int cur_zpos);

	const CWorld *m_World;
	CGraphicWorld *m_GraphicWorld;

	vector<CReflection> m_MovingObjectReflections;
	int m_UpdateBodyReflection; //>= 0 when updating a reflection

	int m_FrameCounter;
};

#endif
