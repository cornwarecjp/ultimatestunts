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

/**
  *@author CJP
  */

class CRenderer {
public: 
	CRenderer(const CLConfig &conf, CGraphicWorld *world);
	~CRenderer();

	void Update();
protected:
	CGraphicWorld *m_World;
};

#endif
