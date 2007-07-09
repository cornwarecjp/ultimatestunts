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

#include "guipage.h"
#include "mainpage.h"

/**
  *@author CJP
  */

class CTEGUI : public CGUI  {
public: 
	CTEGUI(CWinSystem *winsys);
	virtual ~CTEGUI();

	virtual void start(); //returns when an exit command is given

protected:
	void updateMenuTexts();

	//The menu starters:
	CString viewMainMenu();
	CString viewLoadMenu();
	CString viewSaveAsMenu();
	CString viewImportMenu();

	//The menu pages
	CMainPage m_MainPage;
	CGUIPage m_LoadPage;
};

#endif
