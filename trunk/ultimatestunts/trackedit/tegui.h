/***************************************************************************
                          tegui.h  -  The Track Editor GUI
                             -------------------
    begin                : su jul 23 2006
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

#ifndef TEGUI_H
#define TEGUI_H

#include "gui.h"

#include "lconfig.h"

#include "guipage.h"

#include "terenderer.h"
#include "tecamera.h"

#include "temanager.h"

/**
  *@author CJP
  */

class CTEGUI : public CGUI  {
public: 
	CTEGUI(const CLConfig &conf, CWinSystem *winsys);
	virtual ~CTEGUI();

	virtual void start(); //returns when an exit command is given

	virtual int onKeyPress(int key);
	virtual int onMouseClick(int x, int y, unsigned int buttons);

protected:
	void updateMenuTexts();

	//The menu starters:
	CString viewMainMenu();
	CString viewLoadMenu();
	CString viewSaveAsMenu();
	CString viewImportMenu();

	//The menu pages
	CGUIPage m_MainPage;
	CGUIPage m_LoadPage;
	CGUIPage m_ImportPage;

	//Data
	CTERenderer *m_Renderer;
	CTECamera *m_Camera;

	CTEManager *m_DataManager;
	CString m_Trackname;

	CString m_ImportDir;

	bool load();
	bool import(const CString &filename);
	bool save();
};

#endif
