/***************************************************************************
                          trackdocument.cpp  -  All edit state data
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

#include "usmacros.h"
#include "timer.h"

#include "routetracker.h"

#include "trackdocument.h"

CTrackDocument *theTrackDocument = NULL;

CTrackDocument::CTrackDocument(CString trackname)
{
	m_CursorX = m_CursorY = m_CursorZ = 0;
	m_Action = NULL;

	m_DataManager = new CTEManager;
	m_FutureTrack = new CEditTrack(m_DataManager);

	m_Trackname = trackname;
	if(!load())
	{
		printf("Error: %s could not be loaded\n", m_Trackname.c_str());
		exit(1);
	}
}

CTrackDocument::~CTrackDocument()
{
	delete m_FutureTrack;
	delete m_DataManager;
}

CEditTrack *CTrackDocument::getCurrentTrack()
{
	return &(m_UndoHistory[m_UndoIndex]);
}

CEditTrack *CTrackDocument::getDisplayedTrack()
{
	unsigned int time = (unsigned int)(4.0*CTimer().getTime());

	if(time & 1)
		return m_FutureTrack;

	return getCurrentTrack();
}

void CTrackDocument::moveCursor(int x, int y, int z)
{
	if(x < 0) x = 0;
	if(z < 0) z = 0;

	CEditTrack *track = getCurrentTrack();
	if(x > track->m_L-1) x = track->m_L-1;
	if(z > track->m_W-1) z = track->m_W-1;

	m_CursorX = x;
	m_CursorY = y;
	m_CursorZ = z;
}

CVector CTrackDocument::getCursorPos()
{
	return CVector(TILESIZE*m_CursorX, VERTSIZE*m_CursorY, TILESIZE*m_CursorZ);
}

void CTrackDocument::applyAction()
{
	if(m_FutureTrack == NULL) return;

	//Take current track as starting point
	*m_FutureTrack = *getCurrentTrack();

	if(m_Action == NULL) return;
	m_Action->doAction(m_FutureTrack);
	CRouteTracker(m_FutureTrack).trackRoutes();
}

void CTrackDocument::commitAction()
{
	//Delete future undo history
	while(m_UndoHistory.size() > m_UndoIndex+1)
		m_UndoHistory.erase(m_UndoHistory.end());

	m_UndoHistory.push_back(*m_FutureTrack);
	m_UndoIndex++;

	//Maybe the action can be done again:
	applyAction();
}

void CTrackDocument::undo()
{
	if(m_UndoIndex > 0)
	{
		m_UndoIndex--;
		applyAction();
	}
}

void CTrackDocument::redo()
{
	if(m_UndoIndex < m_UndoHistory.size()-1)
	{
		m_UndoIndex++;
		applyAction();
	}
}

bool CTrackDocument::load()
{
	m_DataManager->unloadAll();

	/*
	unsigned int ID = m_DataManager->loadObject(
		CString("tracks/") + m_Trackname,
		CParamList(), CDataObject::eTrack);

	if(ID < 0) return false;
	*/

	CEditTrack track(m_DataManager);
	if(!track.load(m_Trackname, CParamList()))
		return false; //loading failed

	track.sortPillars();

	//This will be done at the first action:
	//By not doing it now, we allow the user
	//to see the original routes.
	//CRouteTracker(&track).trackRoutes();

	m_UndoHistory.clear();
	m_UndoHistory.push_back(track);
	m_UndoIndex = 0;

	m_CursorX = m_CursorY = m_CursorZ = 0;
	applyAction();

	return true;
}

bool CTrackDocument::import(const CString &filename)
{
	m_DataManager->unloadAll();

	{
		CEditTrack track(m_DataManager);
		m_UndoHistory.clear();
		m_UndoHistory.push_back(track);
		m_UndoIndex = 0;
	}

	CEditTrack *track = getCurrentTrack();
	if(track == NULL) return false;

	bool ret = track->import(filename);

	m_CursorX = m_CursorY = m_CursorZ = 0;
	applyAction();

	if(ret)
		deleteUnusedTiles();

	return ret;
}

bool CTrackDocument::save()
{
	CEditTrack *track = getCurrentTrack();
	if(track == NULL) return false;

	track->minimizeHeight();

	return track->save(m_Trackname);
}

bool CTrackDocument::deleteTile(int ID)
{
	if(ID <= 0) return false;

	//Remove tile from tile list
	m_DataManager->unloadObject(CDataObject::eTileModel, ID);

	//Get track object
	CEditTrack track(*getCurrentTrack());

	//Update track array
	for(int i=0; i < track.m_L*track.m_W*track.m_H; i++)
	{
		STile &t = track.m_Track[i];

		if(t.m_Model < ID) continue;
		if(t.m_Model == ID)
		{
			t.m_Model = 0; //empty tile
			continue;
		}
		t.m_Model--;
	}

	//Clear undo history
	m_UndoHistory.clear();
	m_UndoHistory.push_back(track);
	m_UndoIndex = 0;
	applyAction();

	//Remove unused textures
	m_DataManager->removeUnusedTextures();

	return true;
}

bool CTrackDocument::deleteUnusedTiles()
{
	printf("Removing unused tiles\n");

	unsigned int numTiles = m_DataManager->getNumObjects(CDataObject::eTileModel);
	if(numTiles <= 1) return true;

	//Get track object
	CEditTrack track(*getCurrentTrack());

	//Find out which tiles are used
	vector<bool> isUsed(numTiles, false);
	for(int i=0; i < track.m_L*track.m_W*track.m_H; i++)
	{
		STile &t = track.m_Track[i];
		isUsed[t.m_Model] = true;
	}

	//Don't delete tile 0 (the empty tile)
	isUsed[0] = true;

	//Delete all these tiles, from high to low to preserve later IDs in this loop
	for(int i=numTiles-1; i >= 1; i--)
	{
		if(!isUsed[i])
			m_DataManager->unloadObject(CDataObject::eTileModel, i);
	}

	//Now make the ID translation table
	int decrease = 0;
	vector<int> newID(numTiles, 0);
	for(unsigned int i=0; i < numTiles; i++)
	{
		if(!isUsed[i]) decrease++;
		newID[i] = i - decrease;
	}

	//And translate tile IDs
	for(int i=0; i < track.m_L*track.m_W*track.m_H; i++)
	{
		STile &t = track.m_Track[i];
		t.m_Model = newID[t.m_Model];
	}

	//Clear undo history
	m_UndoHistory.clear();
	m_UndoHistory.push_back(track);
	m_UndoIndex = 0;
	applyAction();

	//Remove unused textures
	m_DataManager->removeUnusedTextures();

	return true;
}

