/***************************************************************************
                          routetracker.h  -  (Re-)tracking the routes in a track
                             -------------------
    begin                : zo mei 18 2008
    copyright            : (C) 2008 by CJP
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
#ifndef ROUTETRACKER_H
#define ROUTETRACKER_H

#include "edittrack.h"
#include "tetile.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CRouteTracker
{
public:
	CRouteTracker(CEditTrack *track);
	~CRouteTracker();

	void trackRoutes();

protected:
	CEditTrack *m_Track;
	CDataManager *m_DataManager;


	//Tracking functions:

	void trackSingleRoute(unsigned int routenr);

	bool findNextTile(CTrack::CCheckpoint &pos, unsigned int &route, bool &forward) const;

	//Result is true if there was a split:
	bool goBackToLastSplit(
		unsigned int routenr,
		CTrack::CCheckpoint &pos, unsigned int &route, bool &forward
		);

	void removeSplitRoutes(unsigned int routenr);

	//Tool functions:

	CTrack::CCheckpoint getRoutePosition(
		CTrack::CCheckpoint tilepos,
		CTETile::SIntVector relpos,
		int rotation,
		unsigned int extendedPos=0
		) const;

	//Result is a "matching score": higher is better; 0 is non-matching
	unsigned int backtrackScore(
		const CTrack::CCheckpoint currentPos,
		const CTrack::CCheckpoint newPos,
		const CTrack::CCheckpoint backtrackPos
		) const;

	bool isInTrack(const CTrack::CCheckpoint &pos) const;
	bool isFinish(const CTrack::CCheckpoint &pos) const;

	bool tileExists(const CTrack::CCheckpoint &pos) const;
	STile &getTile(const CTrack::CCheckpoint &pos) const;
	const CTETile *getModel(const STile &tile) const;
};

#endif
