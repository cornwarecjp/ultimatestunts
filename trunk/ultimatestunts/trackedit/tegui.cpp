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
#include "edittrack.h"

#include "longmenu.h"
#include "tileselect.h"
#include "objectviewwidget.h"

#include "tegui.h"

CTEGUI::CTEGUI(CWinSystem *winsys) : CGUI(winsys)
{
	{
		vector<CString> conffiles = getDataDirContents("tiles", ".conf");
		for(unsigned int i=0; i < conffiles.size(); i++)
		{
			CDataFile dfile("tiles/" + conffiles[i]);
			CLConfig conf(dfile.useExtern());

			STileDescr tile;
			tile.description = conf.getValue("description", "text");
			tile.glbfile     = conf.getValue("model"      , "glbfile");
			tile.textures    = conf.getValue("model"      , "textures");
			tile.flags       = conf.getValue("model"      , "flags");

			m_TileFiles.push_back(tile);
		}
	}

	m_CurrentTile = -1;

	m_LoadPage.m_Title = _("Select a track");
	m_LoadPage.m_DrawBackground = true;
	CLongMenu *lmenu = new CLongMenu;
	lmenu->m_Xrel = 0.1;
	lmenu->m_Yrel = 0.2;
	lmenu->m_Wrel = 0.8;
	lmenu->m_Hrel = 0.6;
	lmenu->m_Selected = 0;
	lmenu->m_AlignLeft = false;
	m_LoadPage.m_Widgets.push_back(lmenu);


	m_TilesPage.m_Title = _("Change tiles collection");
	m_TilesPage.m_OnlyTopGetsMouseEvents = false;
	CTileSelect *tselect = new CTileSelect;
	tselect->m_Xrel = 0.1;
	tselect->m_Yrel = 0.2;
	tselect->m_Wrel = 0.8;
	tselect->m_Hrel = 0.6;
	m_TilesPage.m_Widgets.push_back(tselect);
	CMenu *menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.1;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.1;
	menu->m_Selected = 0;
	menu->m_AlignLeft = true;
	m_TilesPage.m_Widgets.push_back(menu);


	m_TilePage.m_Title = _("Edit this tile");
	CObjectViewWidget *tileview = new CObjectViewWidget(theTrackDocument->m_DataManager);
	tileview->m_Xrel = 0.1;
	tileview->m_Yrel = 0.2;
	tileview->m_Wrel = 0.8;
	tileview->m_Hrel = 0.6;
	m_TilePage.m_Widgets.push_back(tileview);
	menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.1;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.1;
	menu->m_Selected = 0;
	menu->m_AlignLeft = true;
	m_TilePage.m_Widgets.push_back(menu);

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

	menu = (CMenu *)(m_TilesPage.m_Widgets[1]);
	menu->m_Lines.clear();
	menu->m_Lines.push_back(_("Add a new tile"));
	menu->m_Lines.push_back(_("Return to main menu"));

	menu = (CMenu *)(m_TilePage.m_Widgets[1]);
	menu->m_Lines.clear();
	menu->m_Lines.push_back(_("Change the tile model"));
	menu->m_Lines.push_back(_("Delete this tile"));
	menu->m_Lines.push_back(_("Ready"));
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
		else if(section=="tilesmenu")
			section = viewTilesMenu();
		else if(section=="tilemenu")
			section = viewTileMenu();
		else if(section=="loadtilemenu")
			section = viewLoadTileMenu();
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
		case CIconList::eTiles:
			return "tilesmenu";
		default:
			printf("Error: received Iconbar event with unknown ID\n");
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

CString CTEGUI::viewTilesMenu()
{
	m_ChildWidget = &m_TilesPage;
	if(!m_WinSys->runLoop(this))
		return "mainmenu";

	if(m_TilesPage.m_EventWidget == 0) //tile-triggered
	{
		CTileSelect *tselect = (CTileSelect *)(m_TilesPage.m_Widgets[0]);
		printf("Edit tile %d\n", tselect->getSelection());
		m_CurrentTile = tselect->getSelection();
		return "tilemenu";
	}
	else if(m_TilesPage.m_EventWidget == 1) //menu-triggered
	{
		CMenu *menu = (CMenu *)(m_TilesPage.m_Widgets[1]);
		switch(menu->m_Selected)
		{
		case 0: //New tile
			printf("New tile\n");
			m_CurrentTile = -1;
			return "loadtilemenu";
		case 1: //Return to main menu
		default:
			break;
		}
	}

	return "mainmenu";
}

CString CTEGUI::viewTileMenu()
{
	if(m_CurrentTile < 0)
		return "tilesmenu";

	const CTEManager *manager = theTrackDocument->m_DataManager;
	const CDataObject *tile = manager->getTile(m_CurrentTile);

	CObjectViewWidget *tileview = (CObjectViewWidget *)(m_TilePage.m_Widgets[0]);
	tileview->m_ObjectViewer.m_Objects.clear();
	tileview->m_ObjectViewer.addObject(tile->getFilename(), tile->getParamList(),
			CVector(0,0,0), CMatrix(), false, CDataObject::eTileModel);

	m_ChildWidget = &m_TilePage;
	if(!m_WinSys->runLoop(this))
		return "tilesmenu";

	CMenu *menu = (CMenu *)(m_TilePage.m_Widgets[1]);
	switch(menu->m_Selected)
	{
	case 0: //Change model
		return "loadtilemenu";
	case 1: //Delete
		return "tilemenu"; //NYI
	case 2: //Ready
	default:
		return "tilesmenu";
	}

	return "tilesmenu";
}

CString CTEGUI::viewLoadTileMenu()
{
	m_LoadPage.m_Title = _("Tiles:");
	CMenu *menu = (CMenu *)(m_LoadPage.m_Widgets[0]);

	menu->m_Lines.clear();
	for(unsigned int i=0; i < m_TileFiles.size(); i++)
	{
		menu->m_Lines.push_back(m_TileFiles[i].description);
	}

	m_ChildWidget = &m_LoadPage;
	if(!m_WinSys->runLoop(this))
	{
		if(m_CurrentTile >= 0)
			return "tilemenu";

		return "tilesmenu";
	}

	CTEManager *manager = theTrackDocument->m_DataManager;

	const STileDescr &td = m_TileFiles[menu->m_Selected];
	CString newfile = td.glbfile;
	CString texSubset = manager->getTextureSubset(td.textures);

	printf("Textures: \"%s\"\n", td.textures.c_str());
	printf("Subset: \"%s\"\n", texSubset.c_str());

	CParamList plist;
	SParameter p;
	p.name = "subset";
	p.value = texSubset;
	plist.push_back(p);
	p.name = "flags";
	p.value = td.flags;
	plist.push_back(p);

	if(m_CurrentTile >= 0)
	{
		CDataObject *tile = manager->getTile(m_CurrentTile);
		tile->load(newfile, plist);
	}
	else
	{
		m_CurrentTile = manager->loadObject(newfile, plist, CDataObject::eTileModel);
		if(m_CurrentTile < 0) return "tilesmenu"; //TODO: error message
	}

	return "tilemenu";
}
