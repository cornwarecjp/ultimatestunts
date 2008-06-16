/***************************************************************************
                          edittrack.h  -  An editable track
                             -------------------
    begin                : ma mei 23 2005
    copyright            : (C) 2005 by CJP
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

#ifndef EDITTRACK_H
#define EDITTRACK_H

#include "track.h"
#include "trkfile.h"

/**
  *@author CJP
  */

class CEditTrack : public CTrack  {
public: 
	CEditTrack(CDataManager *manager);
	~CEditTrack();

	bool save(const CString &filename) const;
	bool import(const CString &filename);

	bool tileIsUsed(int ID) const;

	void sortPillars();
	void sortPillar(unsigned int x, unsigned int z);

	struct SMarker
	{
		CTrack::CCheckpoint pos;
		//enum {eRouteError} type;
	};
	vector<SMarker> m_Markers;

protected:
	void placeItem(unsigned int offset, const CString &item);
	void followTRKRoutes(const CTRKFile &file, CTrack::CCheckpoint start, int dir);
	bool undoRoutingFromSplit(vector<CTrack::CCheckpoint> &splitpoints);
	void trackTRKCorners(unsigned char terrain, unsigned char item,
		int &dir, int &height, int &altdir);

	CString hexStr(unsigned char x);

	//Check whether we need a textures section in the saved file
	bool needsTextureSection() const;
};

#endif
