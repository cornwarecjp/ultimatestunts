/***************************************************************************
                          routetracker.cpp  -  (Re-)tracking the routes in a track
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
#include <cstdio>

#include "routetracker.h"

CRouteTracker::CRouteTracker(CEditTrack *track)
{
	m_Track = track;
	m_DataManager = m_Track->getManager();
}


CRouteTracker::~CRouteTracker()
{
}

void CRouteTracker::trackRoutes()
{
	m_Track->m_Routes.clear();

	const unsigned int L = m_Track->m_L;
	const unsigned int W = m_Track->m_W;
	const unsigned int H = m_Track->m_H;

	//Search for start locations:
	CTrack::CCheckpoint start;
	start.x = start.y = start.z = 0;

	bool found = false;
	for(unsigned int x=0; x < L; x++)
	for(unsigned int z=0; z < W; z++)
	for(unsigned int y=0; y < H; y++)
	{
		unsigned int n = y + H * (z+W*x);
		STile &t = m_Track->m_Track[n];
		CDataObject *tmodel = m_DataManager->getObject(CDataObject::eTileModel, t.m_Model);

		bool isStart = tmodel->getParamList().getValue("flags", "").inStr('s') >= 0;
		if(isStart)
		{
			if(found) //already found another one
			{
				printf("Error: found multiple start tiles\n");
				return;
			}

			start.x = x;
			start.y = t.m_Z;
			start.z = z;

			found = true;
		}
	}

	if(!found)
	{
		printf("Error: found no start tiles\n");
		return;
	}

	printf("Start tile: %d, %d, %d\n", start.x, start.y, start.z);

	//Add the start of the first route
	CTrack::CRoute mainRoute;
	mainRoute.finishRoute = mainRoute.finishTile = mainRoute.startRoute = mainRoute.startTile = 0;
	mainRoute.push_back(start);
	m_Track->m_Routes.push_back(mainRoute);

	//Then track all routes (including the other ones that will be added during the process)
	for(unsigned int i=0; i < m_Track->m_Routes.size(); i++)
		trackSingleRoute(i);
}

void CRouteTracker::trackSingleRoute(unsigned int routenr)
{
	if(routenr >= m_Track->m_Routes.size())
	{
		printf("Error: routenr >= number of routes\n");
		return;
	}

	CTrack::CRoute &theRoute = m_Track->m_Routes[routenr];

	if(theRoute.empty())
	{
		printf("Error: route has no start tile\n");
		return;
	}

	CTrack::CCheckpoint currentPos = theRoute[0];
	bool currentIsForward = true; //direction of current route

	unsigned int currentTileRoute = 0;
	if(routenr > 0)
	{
		//Try to guess the current tile route for alternative routes
		currentTileRoute = 1; //TODO: more advanced test
	}

	STile currentTile = getTile(currentPos);
	const CTETile *currentModel = getModel(currentTile);
	if(currentTileRoute >= currentModel->m_Routes.size())
	{
		printf("Error: currentTileRoute >= number of tile routes\n");
		printf("    Tile: %s\n", currentModel->getFilename().c_str());
		printf("    %d >= %d\n", currentTileRoute, currentModel->m_Routes.size());
		return;
	}

	while(true)
	{
		CTrack::CCheckpoint newPos = currentPos;
		unsigned int newTileRoute = currentTileRoute;
		bool newIsForward = currentIsForward;

		if(!findNextTile(newPos, newTileRoute, newIsForward))
		{
			printf("Error: end of route\n");
			return;
		}

		//printf("New pos: %d, %d, %d\n", newPos.x, newPos.y, newPos.z);
		theRoute.push_back(newPos);

		//Check for finish:
		if(isFinish(newPos))
		{
			printf("Route stops at finish\n");
			return;
		}

		STile newTile = getTile(newPos);
		const CTETile *newModel = getModel(newTile);

		//Check for split/join tiles:
		if(newModel->m_RouteType == CTETile::eSplit)
		{
			if(newIsForward)
			{
				printf("Processing split\n");

				CTrack::CRoute newRoute;
				newRoute.finishRoute = newRoute.finishTile = 0;
				newRoute.startRoute = routenr;
				newRoute.startTile = theRoute.size()-1;
				newRoute.push_back(newPos);

				m_Track->m_Routes.push_back(newRoute);

			}
			else
			{
				printf("Processing join\n");

				//Find previous route, if any:
				if(routenr > 0)
				for(unsigned int j=0; j < routenr; j++)
				{
					CTrack::CRoute &prevRoute = m_Track->m_Routes[j];
					for(unsigned int k=0; k < prevRoute.size(); k++)
					if(prevRoute[k] == newPos)
					{
						printf("Route stops at join\n");
						//Found an existing route:
						//We stop tracking this route now
						theRoute.finishRoute = j;
						theRoute.finishTile = k;
						return;
					}
				}
			}
		}

		currentPos = newPos;
		currentTileRoute = newTileRoute;
		currentIsForward = newIsForward;
	}
}

bool CRouteTracker::findNextTile(CTrack::CCheckpoint &pos, unsigned int &route, bool &forward) const
{
	//Current situation:
	CTrack::CCheckpoint currentPos = pos;
	unsigned int currentRoute = route;
	bool currentForward = forward;

	STile &currentTile = getTile(currentPos);
	const CTETile *currentModel = getModel(currentTile);
	
	//The route on the current tile:
	CTETile::SRoute theTileRoute = currentModel->m_Routes[currentRoute];
	
	//Exit points of current tile route:
	vector<CTETile::SIntVector> exitPoints =
		currentForward? theTileRoute.outPos : theTileRoute.inPos;

	//Extended positions are useful for jumps, ramps etc.
	//Currently we go from 0 to 3, but more could be added in the future.
	//See also getRoutePosition for their exact effect.
	for(unsigned int extendedPos=0; extendedPos < 4; extendedPos++)
	for(unsigned int i=0; i < exitPoints.size(); i++)
	{
		pos = getRoutePosition(
			currentPos, exitPoints[i], currentTile.m_R, extendedPos);
	
		//printf("Candidate pos: %d, %d, %d\n", pos.x, pos.y, pos.z);
	
		if(!isInTrack(pos)) continue;
	
		if(pos == currentPos) continue; //invalid

		if(!tileExists(pos)) continue;

		STile newTile = getTile(pos);
		const CTETile *newModel = getModel(newTile);

		//printf("Candidate tile: %s\n", newModel->getFilename().c_str());

		if(newModel->m_Routes.empty()) continue; //don't go if there is no route

		unsigned int bestScore = 0;

		//Check for a double match in the next tile's routes
		for(unsigned int j=0; j < newModel->m_Routes.size(); j++)
		{
			CTETile::SRoute newRoute = newModel->m_Routes[j];
	
			//Check entry points
			for(unsigned int k=0; k < newRoute.inPos.size(); k++)
			{
				CTrack::CCheckpoint backPos =
					getRoutePosition(pos,
					newRoute.inPos[k], newTile.m_R);

				//printf("Entry backpos: %d, %d, %d\n", backPos.x, backPos.y, backPos.z);

				unsigned int score = backtrackScore(currentPos, pos, backPos);

				if(score > bestScore)
				{
					//printf("Found foward: %s\n", newModel->getFilename().c_str());
					forward = true;
					route = j;
					bestScore = score;
				}
			}
	
			//Check exit points
			for(unsigned int k=0; k < newRoute.outPos.size(); k++)
			{
				CTrack::CCheckpoint backPos =
					getRoutePosition(pos,
					newRoute.outPos[k], newTile.m_R);
	
				//printf("Exit backpos: %d, %d, %d\n", backPos.x, backPos.y, backPos.z);

				unsigned int score = backtrackScore(currentPos, pos, backPos);

				if(score > bestScore)
				{
					//printf("Found backward: %s\n", newModel->getFilename().c_str());
					forward = false;
					route = j;
					bestScore = score;
				}
			}
		}

		if(bestScore != 0) return true; //we found a match
	}

	return false; //didn't find a tile
}

unsigned int CRouteTracker::backtrackScore(
	const CTrack::CCheckpoint currentPos,
	const CTrack::CCheckpoint newPos,
	const CTrack::CCheckpoint backtrackPos
	) const
{
	//Exact match:
	if(backtrackPos == currentPos) return 1000;

	//Only y difference:
	if(backtrackPos.x == currentPos.x && backtrackPos.z == currentPos.z) return 10;

	//In constant-x line:
	if(backtrackPos.x == currentPos.x && backtrackPos.x == newPos.x)
	{
		//Between current and new:
		if(backtrackPos.z > currentPos.z && backtrackPos.z < newPos.z) return 1;
		if(backtrackPos.z < currentPos.z && backtrackPos.z > newPos.z) return 1;
	}

	//In constant-z line:
	if(backtrackPos.z == currentPos.z && backtrackPos.z == newPos.z)
	{
		//Between current and new:
		if(backtrackPos.x > currentPos.x && backtrackPos.x < newPos.x) return 1;
		if(backtrackPos.x < currentPos.x && backtrackPos.x > newPos.x) return 1;
	}

	return 0;
}

CTrack::CCheckpoint CRouteTracker::getRoutePosition(
	CTrack::CCheckpoint tilepos,
	CTETile::SIntVector relpos,
	int rotation,
	unsigned int extendedPos
	) const
{

	//Extended positions:
	if(relpos.y == 0)
	{
		switch(extendedPos)
		{
		case 1:
			relpos.y = -1;
			break;
		}
	}


	//Position translation (with rotated vector):

	tilepos.y += relpos.y;

	switch(rotation & 0x3) //rotation modulo 4
	{
	case 0:
		tilepos.x += relpos.x;
		tilepos.z += relpos.z;
		break;
	case 1:
		tilepos.x += relpos.z;
		tilepos.z -= relpos.x;
		break;
	case 2:
		tilepos.x -= relpos.x;
		tilepos.z -= relpos.z;
		break;
	case 3:
		tilepos.x -= relpos.z;
		tilepos.z += relpos.x;
		break;
	}

	return tilepos;
}

bool CRouteTracker::isInTrack(const CTrack::CCheckpoint &pos) const
{
	return
		pos.x >= 0 && pos.x < m_Track->m_L &&
		pos.y >= 0 && pos.y < m_Track->m_W;
}

bool CRouteTracker::isFinish(const CTrack::CCheckpoint &pos) const
{
	const CTETile *model = getModel(getTile(pos));
	return model->getParamList().getValue("flags", "").inStr('f') >= 0;
}

bool CRouteTracker::tileExists(const CTrack::CCheckpoint &pos) const
{
	unsigned int n = m_Track->m_H * (pos.z+m_Track->m_W*pos.x);

	for(int i=0; i < m_Track->m_H; i++)
		if(m_Track->m_Track[n+i].m_Z == pos.y) return true;

	return false;
}

STile &CRouteTracker::getTile(const CTrack::CCheckpoint &pos) const
{
	unsigned int n = m_Track->m_H * (pos.z+m_Track->m_W*pos.x);

	/*
	for(int i=0; i < m_Track->m_H; i++)
	{
		printf("%d: %d\n", m_Track->m_Track[n+i].m_Z, m_Track->m_Track[n+i].m_Model);
	}
	*/

	//Below lowest: return lowest
	if(pos.y < m_Track->m_Track[n].m_Z)
	{
		//printf("Below: %d < %d\n", pos.y, m_Track->m_Track[n].m_Z);
		return m_Track->m_Track[n];
	}

	int i = 0;
	for(; i < m_Track->m_H; i++)
	{
		//Return exact match:
		if(m_Track->m_Track[n+i].m_Z == pos.y && m_Track->m_Track[n+i].m_Model != 0)
		{
			//printf("Exact match\n");
			return m_Track->m_Track[n+i];
		}

		//TODO: between
	}

	//Above highest: return highest
	//printf("Above\n");
	return m_Track->m_Track[n+i-1];
}

const CTETile *CRouteTracker::getModel(const STile &tile) const
{
	return (CTETile *)(m_DataManager->getObject(CDataObject::eTileModel, tile.m_Model));
}

