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
	CRenderWidget(bool enableDepth=true, bool enableFog=true);
	virtual ~CRenderWidget();

	virtual int onRedraw();

	CRenderer *m_Renderer;

protected:
	bool m_EnableDepth, m_EnableFog;

private:

	void enter2DMode();
	void leave2DMode();
};

#endif
