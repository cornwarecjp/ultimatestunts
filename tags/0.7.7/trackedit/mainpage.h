/***************************************************************************
                          mainpage.h  -  The main page of the trackedit interface
                             -------------------
    begin                : do dec 12 2006
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

#ifndef MAINPAGE_H
#define MAINPAGE_H

#include "guipage.h"

#include "temanager.h"

#include "terenderer.h"
#include "tecamera.h"
#include "edittrack.h"

#include "renderwidget.h"
#include "label.h"
#include "iconbar.h"
#include "compass.h"
#include "replaceactionwidget.h"

#include "iconlist.h"

/**
  *@author CJP
  */

class CMainPage : public CGUIPage  {
public: 
	CMainPage();
	~CMainPage();

	virtual int onKeyPress(int key);
	virtual int onMouseClick(int x, int y, unsigned int buttons);
	virtual int onMouseMove(int x, int y, unsigned int buttons);
	virtual int onIdle();

	void resetCameraPosition();
	void updateDocInfo(); //e.g. filename

	//in case it is disabled by someone else,
	//or if there is a new action widget:
	void enableActionWidgetAction();

	CIconList::eIconID getSelection();

protected:
	CRenderWidget *m_RenderWidget;
	CLabel *m_HintLabel, *m_FilanemeLabel;
	CIconBar *m_IconBar;
	CCompass *m_Compass;
	CReplaceActionWidget *m_ActionWidget;

	CTERenderer *m_Renderer;
	CTECamera *m_Camera;

	unsigned int m_NormalNumWidgets;
	CEditTrack *m_TrackCache;

	int m_PrevMouseX, m_PrevMouseY;
};

#endif
