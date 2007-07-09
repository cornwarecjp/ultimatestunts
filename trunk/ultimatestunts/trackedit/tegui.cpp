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
#include "trackdocument.h"

#include "tegui.h"

#include "longmenu.h"
#include "edittrack.h"

CTEGUI::CTEGUI(CWinSystem *winsys) : CGUI(winsys)
{
	m_LoadPage.m_Title = _("Select a track");
	m_LoadPage.m_DrawBackground = true;
	CLongMenu *menu = new CLongMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.2;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.6;
	m_LoadPage.m_Widgets.push_back(menu);
	menu->m_Selected = 0;
	menu->m_AlignLeft = false;

	m_ChildWidget = &m_MainPage;
}

CTEGUI::~CTEGUI()
{
}

void CTEGUI::updateMenuTexts()
{
	m_MainPage.updateDocInfo();

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

	switch(m_MainPage.getSelection())
	{
		case CIconList::eLoad:
			return "loadmenu";
		case CIconList::eSave:
			return "saveasmenu";
		case CIconList::eImport:
			return "importmenu";
		case CIconList::eQuit:
			return "exit";
		case CIconList::eUndo:
			theTrackDocument->undo();
			return "mainmenu";
		case CIconList::eRedo:
			theTrackDocument->redo();
			return "mainmenu";
		default:
			return "mainmenu"; //unknown: do nothing
	}

	return "";
}

CString CTEGUI::viewLoadMenu()
{
	m_ChildWidget = &m_LoadPage;
	if(!m_WinSys->runLoop(this))
		return "mainmenu";

	CMenu *menu = (CMenu *)(m_LoadPage.m_Widgets[0]);

	theTrackDocument->m_Trackname = CString("tracks/") + menu->m_Lines[menu->m_Selected];
	if(!theTrackDocument->load())
	{
		CString msg;
		msg.format(_("Error: %s could not be loaded"), 256, theTrackDocument->m_Trackname.c_str());
		showMessageBox(msg);
		exit(1);
	}

	m_MainPage.resetCameraPosition();

	return "mainmenu";
}

CString CTEGUI::viewImportMenu()
{
	//Let user select a file
	bool cancelled = false;
	CString importedfile = showFileSelect(_("Select the Stunts track"), ".trk", &cancelled);
	if(cancelled) return "mainmenu";

	//Set trackname to filename without directory
	int lastSlash = -1;
	for(unsigned int i=0; i < importedfile.length(); i++)
		if(importedfile[i] == '/') lastSlash = i;
	if(lastSlash < 0)
		{theTrackDocument->m_Trackname = importedfile;}
	else
		{theTrackDocument->m_Trackname = importedfile.mid(lastSlash+1);}

	//Load the file
	if(!theTrackDocument->import(importedfile))
	{
		CString msg;
		msg.format(_("Error: %s could not be loaded"), 256, theTrackDocument->m_Trackname.c_str());
		showMessageBox(msg);
		exit(1);
	}

	m_MainPage.resetCameraPosition();

	return "mainmenu";
}

CString CTEGUI::viewSaveAsMenu()
{
	m_LoadPage.m_Title = _("Tracks:");
	m_ChildWidget = &m_LoadPage;

	CString name = theTrackDocument->m_Trackname;

	//Remove directory
	int pos = name.inStr('/');
	while(pos >= 0)
	{
		name = name.mid(pos+1);
		pos = name.inStr('/');
	}

	//Add .track extension
	if(name.mid(name.length()-6) != ".track")
			name += ".track";

	//Allow user to modify name
	bool cancelled = false;
	name = showInputBox(_("Enter the filename:"), name, &cancelled);

	//Add .track extension again if user deleted it
	if(name.mid(name.length()-6) != ".track")
			name += ".track";

	if(!cancelled)
	{
		CString oldname = theTrackDocument->m_Trackname;

		theTrackDocument->m_Trackname = CString("tracks/") + name;

		if(dataFileExists(theTrackDocument->m_Trackname, true)) //search only locally
		{
			CString q;
			q.format(_("File %s already exists. Overwrite?"), 256, theTrackDocument->m_Trackname.c_str());
			if(!showYNMessageBox(q)) return "mainmenu"; //and don't save
		}

		if(!theTrackDocument->save())
		{
			CString msg;
			msg.format(_("Saving of %s failed"), 256, theTrackDocument->m_Trackname.c_str());
			showMessageBox(msg);
			theTrackDocument->m_Trackname = oldname;
		}
	}

	return "mainmenu";
}

