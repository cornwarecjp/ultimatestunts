/***************************************************************************
                          trackdocument.h  -  All edit state data
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

#ifndef TRACKDOCUMENT_H
#define TRACKDOCUMENT_H

#include <vector> //STL vector template
namespace std {}
using namespace std;

#include "edittrack.h"
#include "action.h"

#include "temanager.h"

/**
  *@author CJP
  */

class CTrackDocument  {
public: 
	CTrackDocument();
	~CTrackDocument();

	CEditTrack *getCurrentTrack();
	CEditTrack *getDisplayedTrack();

	CTEManager *m_DataManager;

	CString m_Trackname;

	void moveCursor(int x, int y, int z);

	int getCursorX(){return m_CursorX;}
	int getCursorY(){return m_CursorY;}
	int getCursorZ(){return m_CursorZ;}
	CVector getCursorPos();

	void applyAction(); //applies the active action to m_FutureTrack
	void commitAction(); //actually does the action
	CAction *m_Action; //memory-managed from the outside
	void setNewAction(CAction *a)
	{
		if(m_Action != NULL) delete m_Action;
		m_Action = a;
		applyAction();
	}

	void undo();
	void redo();

	bool load();
	bool import(const CString &filename);
	bool save();

protected:
	vector<CEditTrack> m_UndoHistory;
	unsigned int m_UndoIndex;

	CEditTrack *m_FutureTrack;

	int m_CursorX, m_CursorY, m_CursorZ;
};

extern CTrackDocument *theTrackDocument;
#endif
