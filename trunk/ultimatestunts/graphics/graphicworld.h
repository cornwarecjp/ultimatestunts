/***************************************************************************
                          graphicworld.h  -  A graphical version of a world object
                             -------------------
    begin                : do jan 16 2003
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

#ifndef GRAPHICWORLD_H
#define GRAPHICWORLD_H

#include "background.h"
#include "world.h"

/**
  *@author CJP
  */

class CGraphicWorld : public CWorld  {
public: 
	CGraphicWorld(const CLConfig &conf);
	virtual ~CGraphicWorld();

	CBackground m_Background;
protected:
	virtual CShape *createShape();
	virtual CBound *createBound();
	virtual CMaterial *createMaterial();
	virtual bool loadBackground(const CString &descr);
};

#endif
