/***************************************************************************
                          terenderer.h  -  Rendering class of the track editor
                             -------------------
    begin                : ma mei 23 2005
    copyright            : (C) 2005 by CJP
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

#ifndef TERENDERER_H
#define TERENDERER_H

#include "renderer.h"
#include "winsystem.h"
#include "temanager.h"

/**
  *@author CJP
  */

class CTERenderer : public CRenderer  {
public: 
	CTERenderer(const CWinSystem *winsys);
	~CTERenderer();

	void setManager(CTEManager *manager);

	void update();

protected:
	CTEManager *m_Manager;

	int camx, camy, camz; //tile position of the camera
	int tgtx, tgty, tgtz; //tile position of the target

	void drawTrack();
	void viewTrackPart(
		int xmin,int ymin,
		int xmax,int ymax,
		int dx,  int dy,
		int cur_zpos);

	void viewPilaar(int x, int y, int cur_zpos);
};

#endif
