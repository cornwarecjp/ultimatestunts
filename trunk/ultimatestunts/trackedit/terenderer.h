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
#include "temanager.h"
#include "edittrack.h"

/**
  *@author CJP
  */

class CTERenderer : public CRenderer  {
public: 
	CTERenderer();
	~CTERenderer();

	void update();

protected:
	virtual void updateScreenSize();

	int camx, camy, camz; //tile position of the camera
	int tgtx, tgty, tgtz; //tile position of the target

	CEditTrack *m_TrackCache;

	void drawTrack();
	void viewTrackPart(
		int xmin,int ymin,
		int xmax,int ymax,
		int dx,  int dy,
		int cur_zpos);

	void viewPilaar(int x, int y, int cur_zpos);
};

#endif
