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
#include "graphicsettings.h"

/**
  *@author CJP
  */

class CRenderer {
public: 
	CRenderer(const CWinSystem *winsys);
	virtual ~CRenderer();

	virtual bool reloadConfiguration();

	void setCamera(const CCamera *cam)
		{m_Camera = cam;}

	virtual void update(); //default: set up viewport

	unsigned int m_X, m_Y, m_W, m_H; //to be updated from outside or by updateScreenSize
protected:
	virtual void updateScreenSize(); //default: get from winsys

	const CCamera *m_Camera;
	const CWinSystem *m_WinSys;

	float *m_FogColor;

	SGraphicSettings m_Settings;
};

#endif
