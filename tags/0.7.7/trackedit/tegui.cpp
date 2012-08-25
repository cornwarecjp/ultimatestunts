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
#include <cmath>
#include <unistd.h>

#include <libintl.h>
#define _(String) gettext (String)
#define N_(String1, String2, n) ngettext ((String1), (String2), (n))

#include "pi.h"
#include "lconfig.h"
#include "usmacros.h"
#include "datafile.h"
#include "trackdocument.h"
#include "edittrack.h"
#include "environmentaction.h"
#include "resizeaction.h"

#include "longmenu.h"
#include "tileselect.h"
#include "objectviewwidget.h"
#include "scenerypreview.h"

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
			tile.conffile = "tiles/" + conffiles[i];
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

	m_SceneryPage.m_Title = _("Change scenery settings");
	CSceneryPreview *preview = new CSceneryPreview;
	preview->m_Xrel = 0.3;
	preview->m_Yrel = 0.5;
	preview->m_Wrel = 0.4;
	preview->m_Hrel = 0.3;
	m_SceneryPage.m_Widgets.push_back(preview);
	menu = new CMenu;
	menu->m_Xrel = 0.1;
	menu->m_Yrel = 0.1;
	menu->m_Wrel = 0.8;
	menu->m_Hrel = 0.4;
	menu->m_Selected = 0;
	menu->m_AlignLeft = true;
	m_SceneryPage.m_Widgets.push_back(menu);

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
	menu->m_Selected = 0;

	menu = (CMenu *)(m_TilesPage.m_Widgets[1]);
	menu->m_Lines.clear();
	menu->m_Lines.push_back(_("Add a new tile"));
	menu->m_Lines.push_back(_("Remove unused tiles"));
	menu->m_Lines.push_back(_("Return to main menu"));

	menu = (CMenu *)(m_TilePage.m_Widgets[1]);
	menu->m_Lines.clear();
	menu->m_Lines.push_back(_("Change the tile model"));
	menu->m_Lines.push_back(_("Delete this tile"));
	menu->m_Lines.push_back(_("Ready"));

	menu = (CMenu *)(m_SceneryPage.m_Widgets[1]);
	menu->m_Lines.clear();
	menu->m_Lines.push_back(_("Change the cloud image"));
	menu->m_Lines.push_back(_("Change the background image"));
	menu->m_Lines.push_back(_("Change the sky color"));
	menu->m_Lines.push_back(_("Change the horizon sky color"));
	menu->m_Lines.push_back(_("Change the cloud and background color"));
	menu->m_Lines.push_back(_("Change the fog color"));
	menu->m_Lines.push_back(_("Change the light color"));
	menu->m_Lines.push_back(_("Change the light direction"));
	menu->m_Lines.push_back(_("Return to main menu"));
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
		else if(section=="scenerymenu")
			section = viewSceneryMenu();
		else
			{printf("Error: unknown menu\n");}

		if(section == "exit" || section == "")
		{
			if(showYNMessageBox(_("Do you really want to quit?")) )
				{break;}
			else
				{section = "mainmenu";}
		}
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
		case CIconList::eScenery:
			return "scenerymenu";
		case CIconList::eResizeTrack:
			resizeTrack();
			return "mainmenu";
		default:
			printf("Error: received Iconbar event with unknown ID\n");
			return "mainmenu"; //unknown: do nothing
	}

	return "";
}

CString CTEGUI::viewLoadMenu()
{
	m_LoadPage.m_Title = _("Select a track");
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
			q.format(_("File %s already exists. Overwrite? The hiscore of the track will be reset."), 256, theTrackDocument->m_Trackname.c_str());
			if(!showYNMessageBox(q)) return "mainmenu"; //and don't save
		}

		if(theTrackDocument->save())
		{
			//Remove the hiscore file
			CString hiscoreFile = theTrackDocument->m_Trackname;
			hiscoreFile =
				hiscoreFile.mid(0, hiscoreFile.length()-6) + ".high";
			deleteDataFile(hiscoreFile);
		}
		else
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

		if(theTrackDocument->getCurrentTrack()->tileIsUsed(m_CurrentTile))
		{
			printf("Tile is used\n");

			bool cancelled = false;
			if(showYNMessageBox(
				_("This tile is in use in the track. Do you really want to change it?")
				, &cancelled) && !cancelled)
			{
				return "tilemenu"; //change tile
			}

			return "tilesmenu"; //return to this menu
		}

		return "tilemenu"; //change tile
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
		case 1: //Delete unused tiles
			theTrackDocument->deleteUnusedTiles();
			return "tilesmenu";
		case 2: //Return to main menu
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
		if(theTrackDocument->getCurrentTrack()->tileIsUsed(m_CurrentTile))
		{

			bool cancelled = false;
			if(showYNMessageBox(
				_("This tile is in use in the track. Do you really want to delete it?")
				, &cancelled) && !cancelled)
			{
				theTrackDocument->deleteTile(m_CurrentTile);
				return "tilesmenu";
			}

			//Don't remove it
			return "tilemenu";
		}

		theTrackDocument->deleteTile(m_CurrentTile);
		return "tilesmenu";
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
	CString newfile = td.conffile;
	/*
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
	*/

	if(m_CurrentTile >= 0)
	{
		CDataObject *tile = manager->getTile(m_CurrentTile);
		tile->load(newfile, CParamList());
	}
	else
	{
		m_CurrentTile = manager->loadObject(newfile, CParamList(), CDataObject::eTileModel);
		if(m_CurrentTile < 0) return "tilesmenu"; //TODO: error message
	}

	return "tilemenu";
}

CString CTEGUI::viewSceneryMenu()
{
	CSceneryPreview *preview =
		(CSceneryPreview *)(m_SceneryPage.m_Widgets[0]);

	static bool enteredFromOutside = true;

	if(enteredFromOutside)
		preview->m_Environment =
			theTrackDocument->getCurrentTrack()->m_Environment;

	bool toMainMenu = false;

	m_ChildWidget = &m_SceneryPage;
	if(!m_WinSys->runLoop(this))
		toMainMenu = true;

	if(!toMainMenu)
	{
		CMenu *menu = (CMenu *)(m_SceneryPage.m_Widgets[1]);
		switch(menu->m_Selected)
		{
		case 0: //cloud image
			preview->m_Environment.m_SkyFilename =
				showInputBox(_("Cloud image file:"),
					preview->m_Environment.m_SkyFilename);
			break;
		case 1: //background image
			preview->m_Environment.m_HorizonFilename =
				showInputBox(_("Background image file:"),
					preview->m_Environment.m_HorizonFilename);
			break;
		case 2: //sky color
			preview->m_Environment.m_SkyColor =
				showColorSelect(_("Sky color:"),
					preview->m_Environment.m_SkyColor);
			break;
		case 3: //horizon sky color
			preview->m_Environment.m_HorizonSkyColor =
				showColorSelect(_("Horizon sky color:"),
					preview->m_Environment.m_HorizonSkyColor);
			break;
		case 4: //cloud and background color
			preview->m_Environment.m_EnvironmentColor =
				showColorSelect(_("Cloud and background color:"),
					preview->m_Environment.m_EnvironmentColor);
			break;
		case 5: //fog color
			preview->m_Environment.m_FogColor =
				showColorSelect(_("Fog color:"),
					preview->m_Environment.m_FogColor);
			break;
		case 6: //light color
			preview->m_Environment.m_LightColor =
				showColorSelect(_("Light color:"),
					preview->m_Environment.m_LightColor);
			break;
		case 7: //light direction
			{
				float lightY = asin(
					-preview->m_Environment.m_LightDirection.normal().y
					);
				float lightX = atan2f(
					-preview->m_Environment.m_LightDirection.x,
					 preview->m_Environment.m_LightDirection.z);
				lightX *= 180.0 / M_PI;
				lightY *= 180.0 / M_PI;
				if(lightX < 0.0) lightX += 360.0;

				//printf("%s\n", CString(
				//	preview->m_Environment.m_LightDirection).c_str()
				//	);
				//printf("%f, %f\n", lightX, lightY);

				lightX =
					showInputBox(_("Light compass direction (degrees):"),
						CString(lightX)).toFloat();

				lightY =
					showInputBox(_("Light angle above the horizon (degrees):"),
						CString(lightY)).toFloat();

				//printf("%f, %f\n", lightX, lightY);

				lightX *= M_PI/180.0;
				lightY *= M_PI/180.0;

				preview->m_Environment.m_LightDirection =
					CVector(
						-cos(lightY)*sin(lightX),
						-sin(lightY),
						cos(lightY)*cos(lightX)
					);

				//printf("%s\n", CString(
				//	preview->m_Environment.m_LightDirection).c_str()
				//	);
			}
			break;
		case 8: //main menu
			toMainMenu = true;
			break;
		}
	}

	if(toMainMenu)
	{
		//Apply the changes to the track:
		CEnvironmentAction a(preview->m_Environment);
		theTrackDocument->setNewAction(&a);
		theTrackDocument->commitAction();

		m_MainPage.enableActionWidgetAction();

		enteredFromOutside = true;
		return "mainmenu";
	}

	enteredFromOutside = false;
	return "scenerymenu";
}

void CTEGUI::resizeTrack()
{
	bool cancelled = false;
	int dx_min = showInputBox(
		_("Number of tiles to add on the west side (negative = remove tiles): "),
		"0", &cancelled).toInt();
	if(cancelled) return;
	int dx_max = showInputBox(
		_("Number of tiles to add on the east side (negative = remove tiles): "),
		"0", &cancelled).toInt();
	if(cancelled) return;
	int dz_min = showInputBox(
		_("Number of tiles to add on the north side (negative = remove tiles): "),
		"0", &cancelled).toInt();
	if(cancelled) return;
	int dz_max = showInputBox(
		_("Number of tiles to add on the south side (negative = remove tiles): "),
		"0", &cancelled).toInt();
	if(cancelled) return;

	//Apply the changes to the track:
	CResizeAction a(dx_min, dx_max, dz_min, dz_max);
	theTrackDocument->setNewAction(&a);
	theTrackDocument->commitAction();

	//Move cursor inside the new track
	theTrackDocument->moveCursor(
		theTrackDocument->getCursorX() + dx_min,
		theTrackDocument->getCursorY(),
		theTrackDocument->getCursorZ() + dz_min
		);

	//TODO: find some way to update the camera

	//Enable the original action
	m_MainPage.enableActionWidgetAction();
}


