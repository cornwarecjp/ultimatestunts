/***************************************************************************
                          renderwidget.h  -  GUI widget for 3D rendering
                             -------------------
    begin                : sa jul 22 2006
    copyright            : (C) 2006 by CJP
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

#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include "widget.h"
#include "renderer.h"

class CRenderWidget : public CWidget {
public:
	CRenderWidget(bool enableDepth=true, bool enableFog=true, bool idleRedraw=false);
	virtual ~CRenderWidget();

	void setIdleRedraw(bool b)
		{m_IdleRedraw = b;}

	virtual int onRedraw();
	virtual int onIdle();

	CRenderer *m_Renderer;

protected:
	bool m_EnableDepth, m_EnableFog, m_IdleRedraw;

private:

	void enter2DMode();
	void leave2DMode();
};

//Static interfaces for use in non-CRenderWidget-derived code:
void _clear3DArea(unsigned int w, unsigned int h);
void _leave2DMode(bool depth, bool fog);
void _enter2DMode(unsigned int x, unsigned int y, bool depth, bool fog);

#endif
