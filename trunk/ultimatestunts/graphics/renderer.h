/***************************************************************************
                          renderer.h  -  The graphics rendering routines
                             -------------------
    begin                : di jan 28 2003
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

#ifndef RENDERER_H
#define RENDERER_H

#include "lconfig.h"
#include "graphicworld.h"
#include "camera.h"

#include "car.h"

/**
  *@author CJP
  */

class CRenderer {
public: 
	CRenderer(const CLConfig &conf, const CWorld *world);
	~CRenderer();

	void setCamera(const CCamera *cam);

	bool loadTrackData();
	void unloadTrackData();
	bool loadObjData();
	void unloadObjData();

	void update();
	void viewBackground();
	void viewCar(CCar *car);
	void viewPilaar(int x, int y, int cur_zpos);
protected:
	const CWorld *m_World;
	CGraphicWorld *m_GraphicWorld;

	const CCamera *m_Camera;

	float *m_FogColor;

	bool m_UseBackground;
	bool m_ZBuffer;
	int m_VisibleTiles;
	int m_FogMode;
	bool m_TexPerspective;
	bool m_TexSmooth;
	bool m_ShadowSmooth;

	void viewTrackPart(
		int xmin,int ymin,
		int xmax,int ymax,
		int dx,  int dy,
		int cur_zpos);
};

#endif
