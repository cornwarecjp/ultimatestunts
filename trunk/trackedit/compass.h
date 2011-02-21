/***************************************************************************
                          compass.h  -  A compass for navigating through the world
                             -------------------
    begin                : ma sep 24 2007
    copyright            : (C) 2007 by CJP
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
#ifndef CCOMPASS_H
#define CCOMPASS_H

#include "widget.h"
#include "renderer.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/

class CCompass : public CWidget, CRenderer
{
public:
	CCompass();
	~CCompass();

	//CRenderer wrappers:
	void setCamera(const CCamera *cam)
		{CRenderer::setCamera(cam);}

	virtual int onMouseMove(int x, int y, unsigned int buttons);
	virtual int onMouseClick(int x, int y, unsigned int buttons);
	virtual int onRedraw();

	enum eSelection {eNone, eXup, eXdn, eYup, eYdn, eZup, eZdn} m_Selection;

protected:
	virtual void updateScreenSize();

	void setColor(bool selected, bool north=false);
	void drawCompass();
	void drawUpDown();
};

#endif
