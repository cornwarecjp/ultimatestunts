/***************************************************************************
                          tegui.cpp  -  The Track Editor GUI
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

#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include <libintl.h>
#define _(String) gettext (String)
#define N_(String1, String2, n) ngettext ((String1), (String2), (n))

#include "lconfig.h"
#include "usmacros.h"
#include "datafile.h"

#include "tegui.h"

#include "renderwidget.h"
#include "longmenu.h"

#include "edittrack.h"

CTEGUI::CTEGUI(const CLConfig &conf, CWinSystem *winsys) : CGUI(conf, winsys)
{
	m_ImportDir = "./";

	m_DataManager = new CTEManager;

	m_Trackname = "default.track";
	if(!load())
	{
		printf("Error: %s could not be loaded\n", m_Trackname.c_str());
		exit(1);
	}

	printf("\nInitialising the rendering engine\n");
	m_Renderer = new CTERenderer(winsys);
	m_Camera = new CTECamera();
	m_Renderer->setCamera(m_Camera);
	m_Renderer->setManager(m_DataManager);


	m_ChildWidget = &m_MainPage;


	m_MainPage.m_Title = m_Trackname;
	m_MainPage.m_DrawBackground = false;

	CRenderWidget *render = new CRenderWidget;
	render->m_Xrel = 0.3;
	render->m_Yrel = 0.0;
	render->m_Wrel = 0.7;
	render->m_Hrel = 0.8;
	render->m_Renderer = m_Renderer;
	m_MainPage.m_Widgets.push_back(render);

	CMenu *menu = new CMenu;
	menu->m_Xrel = 0.0;
	menu->m_Yrel = 0.0;
	menu->m_Wrel = 0.3;
	menu->m_Hrel = 0.8;
	menu->m_Lines.push_back(_("Load track"));
	menu->m_Lines.push_back(_("Save track"));
	menu->m_Lines.push_back(_("Import Stunts track"));
	menu->m_Lines.push_back(_("Exit"));
	menu->m_AlignLeft = false;
	m_MainPage.m_Widgets.push_back(menu);

	m_LoadPage.m_Title = _("Select a track:");
	m_LoadPage.m_DrawBackground = true;
	menu = new CLongMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_LoadPage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = false;

	m_ImportPage.m_Title = _("Select a track:");
	m_ImportPage.m_DrawBackground = true;
	menu = new CLongMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_ImportPage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = false;
}

CTEGUI::~CTEGUI()
{
	delete m_Renderer;
	delete m_Camera;

	delete m_DataManager;
}

void CTEGUI::updateMenuTexts()
{
	m_MainPage.m_Title = m_Trackname;

	CMenu *menu = (CMenu *)(m_LoadPage.m_Widgets[0]);
	menu->m_Lines = getDataDirContents("tracks", ".track");
}

void CTEGUI::start()
{
	enter2DMode();

	CString section = "mainmenu";
	while(true)
	{
		updateMenuTexts();

		if(section=="mainmenu")
			section = viewMainMenu();
		else if(section=="loadmenu")
			section = viewLoadMenu();
		else if(section=="saveasmenu")
			section = viewSaveAsMenu();
		else if(section=="importmenu")
			section = viewImportMenu();
		else
			{printf("Error: unknown menu\n");}

		if(section == "exit" || section == "")
			if(showYNMessageBox(_("Do you really want to quit?")) )
				{break;}
			else
				{section = "mainmenu";}
	}

	leave2DMode();
}

CString CTEGUI::viewMainMenu()
{
	m_ChildWidget = &m_MainPage;
	if(!m_WinSys->runLoop(this))
		return "exit";


	CMenu *menu = (CMenu *)(m_MainPage.m_Widgets[1]);

	switch(menu->m_Selected)
	{
		case 0:
			return "loadmenu";
		case 1:
			return "saveasmenu";
		case 2:
			return "importmenu";
		case 3:
			return "exit";
	}

	return "";
}

CString CTEGUI::viewLoadMenu()
{
	m_ChildWidget = &m_LoadPage;
	if(!m_WinSys->runLoop(this))
		return "mainmenu";

	CMenu *menu = (CMenu *)(m_LoadPage.m_Widgets[0]);

	m_Trackname = menu->m_Lines[menu->m_Selected];
	if(!load())
	{
		CString msg;
		msg.format(_("Error: %s could not be loaded"), 256, m_Trackname.c_str());
		showMessageBox(msg);
		exit(1);
	}

	m_Camera->resetPosition();

	return "mainmenu";
}

CString CTEGUI::viewImportMenu()
{
	bool cancelled = false;
	CString newDir = showInputBox(
		_("Enter the directory with Stunts tracks:"), m_ImportDir, &cancelled);

	if(cancelled) return "mainmenu";

	if(newDir[newDir.length()-1] != '/') newDir += '/';

	if(!dirExists(newDir))
	{
		CString msg;
		msg.format(_("The directory %s does not exist"), 256, newDir.c_str());
		showMessageBox(msg);
		return "mainmenu";
	}

	m_ImportDir = newDir;

	CMenu *menu = (CMenu *)(m_ImportPage.m_Widgets[0]);
	menu->m_Lines = getDirContents(m_ImportDir, ".trk");

	m_ChildWidget = &m_ImportPage;
	if(!m_WinSys->runLoop(this))
		return "mainmenu";

	m_Trackname = menu->m_Lines[menu->m_Selected];
	if(!import(m_ImportDir + m_Trackname))
	{
		CString msg;
		msg.format(_("Error: %s could not be loaded"), 256, m_Trackname.c_str());
		showMessageBox(msg);
		exit(1);
	}

	m_Camera->resetPosition();

	return "mainmenu";
}

CString CTEGUI::viewSaveAsMenu()
{
	m_LoadPage.m_Title = _("Tracks:");
	m_ChildWidget = &m_LoadPage;

	bool cancelled = false;
	CString name = showInputBox(_("Enter the filename:"), m_Trackname, &cancelled);

	if(!cancelled)
	{
		CString oldname = m_Trackname;

		if(name.mid(name.length()-6) != ".track")
				name += ".track";
		m_Trackname = name;

		if(dataFileExists(CString("tracks/") + m_Trackname, true)) //search only locally
		{
			CString q;
			q.format(_("File %s already exists. Overwrite?"), 256, m_Trackname.c_str());
			if(!showYNMessageBox(q)) return "mainmenu"; //and don't save
		}

		if(!save())
		{
			CString msg;
			msg.format(_("Saving of %s failed"), 256, m_Trackname.c_str());
			showMessageBox(msg);
			m_Trackname = oldname;
		}
	}

	return "mainmenu";
}

int CTEGUI::onKeyPress(int key)
{
	if(m_ChildWidget == &m_MainPage && m_MainPage.m_Widgets.size()==2)
	{
		switch(key)
		{
		case SDLK_PAGEUP:
			m_Camera->turnUp(0.05);
			break;
		case SDLK_PAGEDOWN:
			m_Camera->turnUp(-0.05);
			break;
		case SDLK_LEFT:
			m_Camera->turnRight(-0.05);
			break;
		case SDLK_RIGHT:
			m_Camera->turnRight(0.05);
			break;
		case SDLK_UP:
			m_Camera->moveForward(50.0);
			break;
		case SDLK_DOWN:
			m_Camera->moveForward(-50.0);
			break;
		}

		return WIDGET_REDRAW;
	}

	return CGUI::onKeyPress(key);
}

int CTEGUI::onMouseClick(int x, int y, unsigned int buttons)
{
	return CGUI::onMouseClick(x, y, buttons);

	//TODO: maybe mouse-dragging in main menu
}

bool CTEGUI::load()
{
	m_DataManager->unloadAll();

	unsigned int ID = m_DataManager->loadObject(
		CString("tracks/") + m_Trackname,
		CParamList(), CDataObject::eTrack);

	if(ID < 0) return false; //loading failed

	return true;
}

bool CTEGUI::import(const CString &filename)
{
	CEditTrack *track = m_DataManager->getTrack();

	if(track == NULL) return false;

	return track->import(filename);
}

bool CTEGUI::save()
{
	const CEditTrack *track = m_DataManager->getTrack();

	if(track == NULL) return false;

	return track->save(CString("tracks/") + m_Trackname);
}
