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
#include "camera.h"
#include "winsystem.h"

/**
  *@author CJP
  */

class CRenderer {
public: 
	CRenderer(const CWinSystem *winsys);
	virtual ~CRenderer();

	void setCamera(const CCamera *cam)
		{m_Camera = cam;}

	virtual void update(); //default: set up viewport

protected:
	const CCamera *m_Camera;
	const CWinSystem *m_WinSys;

	float *m_FogColor;

	bool m_UseBackground;
	bool m_ZBuffer;
	int m_VisibleTiles;
	int m_FogMode;
	int m_MovingObjectLOD;
	enum {off, blend} m_Transparency;
	bool m_TexSmooth;
	bool m_ShadowSmooth;
	float m_ReflectionDist;
	int m_ReflectionSize;
	int m_ReflectionUpdateFrames;
	bool m_ReflectionSkipMovingObjects;
};

#endif
