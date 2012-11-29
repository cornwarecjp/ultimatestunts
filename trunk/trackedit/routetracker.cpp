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
	m_Track->m_Markers.clear();

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

				CEditTrack::SMarker marker;

				marker.pos = start;
				m_Track->m_Markers.push_back(marker);

				marker.pos.x = x;
				marker.pos.y = t.m_Z;
				marker.pos.z = z;
				m_Track->m_Markers.push_back(marker);

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

	//printf("Start tile: %d, %d, %d\n", start.x, start.y, start.z);

	//Add the start of the first route
	CTrack::CRoute mainRoute;
	mainRoute.finishRoute = mainRoute.finishTile = mainRoute.startRoute = mainRoute.startTile = 0;
	mainRoute.push_back(start);
	m_Track->m_Routes.push_back(mainRoute);

	//Then track all routes (including the other ones that will be added during the process)
	for(unsigned int i=0; i < m_Track->m_Routes.size(); i++)
		trackSingleRoute(i);

	//Then erase all empty routes:
	for(unsigned int i=0; i < m_Track->m_Routes.size(); i++)
		if(m_Track->m_Routes[i].empty())
		{
			m_Track->m_Routes.erase(m_Track->m_Routes.begin() + i);
			i--;
		}
}

void CRouteTracker::trackSingleRoute(unsigned int routenr)
{
	//printf("\nStart of route %d\n", routenr);

	if(routenr >= m_Track->m_Routes.size())
	{
		printf("Error: routenr >= number of routes\n");
		return;
	}

	CTrack::CRoute *theRoute = &(m_Track->m_Routes[routenr]);

	if(theRoute->empty())
	{
		printf("Error: route has no start tile\n");
		return;
	}

	CTrack::CCheckpoint currentPos = (*theRoute)[0];
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
		printf("    %d >= %lu\n", currentTileRoute, static_cast<unsigned long>(currentModel->m_Routes.size()));
		return;
	}

	while(true)
	{
		CTrack::CCheckpoint newPos = currentPos;
		unsigned int newTileRoute = currentTileRoute;
		bool newIsForward = currentIsForward;

		bool hasReturnedToSplit = false;

		if(!findNextTile(newPos, newTileRoute, newIsForward))
		{
			//printf("End of route\n");

			CEditTrack::SMarker marker;
			marker.pos = currentPos;
			m_Track->m_Markers.push_back(marker);

			if(!goBackToLastSplit(routenr, newPos, newTileRoute, newIsForward))
			{
				//printf("Route is invalid\n");

				//Route is invalid.
				//Remove it if it's not the primary route:
				if(routenr != 0) //Only remove non-primary routes:
					theRoute->clear(); //empty routes will be deleted

				return;
			}

			//printf("Returned to last split\n");

			//Update the route pointer, because goBackToLastSplit
			//makes changes to the route array:
			theRoute = &(m_Track->m_Routes[routenr]);

			hasReturnedToSplit = true;
		}

		//printf("New pos: %d, %d, %d\n", newPos.x, newPos.y, newPos.z);
		theRoute->push_back(newPos);

		//Check for finish:
		if(isFinish(newPos))
		{
			//printf("Route stops at finish\n");
			theRoute->finishRoute = 0;
			theRoute->finishTile = 0;
			return;
		}

		STile newTile = getTile(newPos);
		const CTETile *newModel = getModel(newTile);

		//Check for splits
		if(newModel->m_RouteType == CTETile::eSplit && newIsForward)
		{
			if(newIsForward && !hasReturnedToSplit)
			{
				//Add new route for alternative direction
				CTrack::CRoute newRoute;
				newRoute.finishRoute = newRoute.finishTile = 0;
				newRoute.startRoute = routenr;
				newRoute.startTile = theRoute->size()-1;
				newRoute.push_back(newPos);
	
				//printf("  Created new route %d: startTile = %d; "
				//	"startPos = %d,%d,%d\n",
				//	m_Track->m_Routes.size(), newRoute.startTile,
				//	newPos.x, newPos.y, newPos.z);

				m_Track->m_Routes.push_back(newRoute);

				//Update the route pointer, because we
				//made a change to the route array:
				theRoute = &(m_Track->m_Routes[routenr]);
			}

			//don't check for joins:
			//move to new position
			currentPos = newPos;
			currentTileRoute = newTileRoute;
			currentIsForward = newIsForward;
			continue;
		}

		//Check for joins:
		for(unsigned int j=0; j <= routenr; j++)
		{
			CTrack::CRoute &prevRoute = m_Track->m_Routes[j];
			for(unsigned int k=1; k+1 < prevRoute.size(); k++)
			if(prevRoute[k] == newPos)
			{
				//printf("Found existing route on the new pos\n");

				//Now check if the join is valid:
				CTrack::CCheckpoint newPos2 = newPos;
				unsigned int newTileRoute2 = newTileRoute;
				bool newIsForward2 = newIsForward;

				if(!findNextTile(newPos2, newTileRoute2, newIsForward2))
				{
					//End of route

					//Go back to the last split,
					//or, if that fails, remove the entire route and return

					if(!goBackToLastSplit(routenr, newPos, newTileRoute, newIsForward))
					{
						//Entire route is invalid.
						//Remove it if it's not the primary route:
						if(routenr != 0) //Only remove non-primary routes:
							{theRoute->clear();} //empty routes will be deleted
						else
						{
							CEditTrack::SMarker marker;
							marker.pos = currentPos;
							m_Track->m_Markers.push_back(marker);
						}

						return;
					}
	
					//Update the route pointer, because we
					//made a change to the route array:
					theRoute = &(m_Track->m_Routes[routenr]);
	
					//Add the last split position again:
					//printf("New pos: %d, %d, %d\n", newPos.x, newPos.y, newPos.z);
					theRoute->push_back(newPos);

					//move to new position
					currentPos = newPos;
					currentTileRoute = newTileRoute;
					currentIsForward = newIsForward;
					continue;
				}

				CTrack::CCheckpoint backwardPos = prevRoute[k-1];
				CTrack::CCheckpoint forwardPos = prevRoute[k+1];

				if(newPos2 == forwardPos) //join in the same direction
				{
					if(j == routenr) //joining itself: this is invalid
					{
						//Go back to the last split,
						//or, if that fails, remove the entire route and return

						if(!goBackToLastSplit(routenr, newPos, newTileRoute, newIsForward))
						{
							//Entire route is invalid.
							//Remove it if it's not the primary route:
							if(routenr != 0) //Only remove non-primary routes:
								{theRoute->clear();} //empty routes will be deleted
							else
							{
								CEditTrack::SMarker marker;
								marker.pos = currentPos;
								m_Track->m_Markers.push_back(marker);
							}

							return;
						}
	
						//Update the route pointer, because we
						//made a change to the route array:
						theRoute = &(m_Track->m_Routes[routenr]);
	
						//Add the last split position again:
						//printf("New pos: %d, %d, %d\n", newPos.x, newPos.y, newPos.z);
						theRoute->push_back(newPos);
					}
					else
					{
						//Stop route at the join
						theRoute->finishRoute = j;
						theRoute->finishTile = k;
						return;
					}
				}
				else if(newPos2 == backwardPos) //join in opposite direction
				{
					//this is inavlid in the current game version

					//Go back to the last split,
					//or, if that fails, remove the entire route and return

					if(!goBackToLastSplit(routenr, newPos, newTileRoute, newIsForward))
					{
						//Entire route is invalid.
						//Remove it if it's not the primary route:
						if(routenr != 0) //Only remove non-primary routes:
							{theRoute->clear();} //empty routes will be deleted
						else
						{
							CEditTrack::SMarker marker;
							marker.pos = currentPos;
							m_Track->m_Markers.push_back(marker);
						}

						return;
					}
	
					//Update the route pointer, because we
					//made a change to the route array:
					theRoute = &(m_Track->m_Routes[routenr]);
	
					//Add the last split position again:
					//printf("New pos: %d, %d, %d\n", newPos.x, newPos.y, newPos.z);
					theRoute->push_back(newPos);
				}
			}
		}

		//move to new position
		currentPos = newPos;
		currentTileRoute = newTileRoute;
		currentIsForward = newIsForward;
	}
}

bool CRouteTracker::goBackToLastSplit(
	unsigned int routenr,
	CTrack::CCheckpoint &pos, unsigned int &route, bool &forward
	)
{
	//printf("goBackToLastSplit on route %d\n", routenr);

	//First find the split-up route number.
	//All candidates are after this route in the route array.
	//The last split is the last item that starts at our route.
	unsigned int splitRoute = 0;
	if(routenr+1 >= m_Track->m_Routes.size()) return false; //There are no split-up routes
	for(unsigned int i = m_Track->m_Routes.size()-1; i != routenr; i--)
		if(m_Track->m_Routes[i].startRoute == routenr)
		{
			splitRoute = i;
			break;
		}
	if(splitRoute == 0) return false; //We didn't find a route splitting from this route

	unsigned int splitTile = m_Track->m_Routes[splitRoute].startTile;

	//printf("Found split route %d, starting on tile %d\n", splitRoute, splitTile);

	CTrack::CRoute &theRoute = m_Track->m_Routes[routenr];

	if(m_Track->m_Routes[splitRoute].size() != 1)
	{
		printf("Bug in route tracking: split-up route does not contain 1 position\n");
		return false;
	}

	if(theRoute[splitTile] != (m_Track->m_Routes[splitRoute])[0])
	{
		printf("Bug in route tracking: split-up route positions don't match\n");
		return false;
	}

	//The return state:
	pos = theRoute[splitTile];
	route = 1; //TODO: more advanced check to find the alternative route
	forward = true;

	//Remove later tiles in this route (including the split, which will be added again later):
	theRoute.resize(splitTile);

	//Remove the split-up route:
	m_Track->m_Routes.erase(m_Track->m_Routes.begin() + splitRoute);

	//printf("goBackToLastSplit finished succesfully\n");

	return true;
}

void CRouteTracker::removeSplitRoutes(unsigned int routenr)
{
	while(true)
	{
		if(routenr+1 >= m_Track->m_Routes.size()) return; //no more split routes

		bool found = false;
		for(unsigned int i=routenr+1; i < m_Track->m_Routes.size(); i++)
			if(m_Track->m_Routes[i].startRoute == routenr)
			{
				m_Track->m_Routes.erase(m_Track->m_Routes.begin() + i);

				found = true;
				break;
			}

		if(!found) return; //no more split routes found
	}
}

bool CRouteTracker::findNextTile(CTrack::CCheckpoint &pos, unsigned int &route, bool &forward) const
{
	//Current situation:
	CTrack::CCheckpoint currentPos = pos;
	unsigned int currentRoute = route;
	bool currentForward = forward;

	STile currentTile = getTile(currentPos);
	const CTETile *currentModel = getModel(currentTile);
	
	//The route on the current tile:
	CTETile::SRoute theTileRoute = currentModel->m_Routes[currentRoute];
	
	//Exit points of current tile route:
	vector<CTETile::SIntVector> exitPoints =
		currentForward? theTileRoute.outPos : theTileRoute.inPos;

	//Extended positions are useful for jumps, ramps etc.
	//Currently we go from 0 to 2, but more could be added in the future.
	//See also getRoutePosition for their exact effect.
	for(unsigned int extendedPos=0; extendedPos < 3; extendedPos++)
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
					//printf("Found forward: %s\n", newModel->getFilename().c_str());
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
		case 2:
			relpos.y = -2;
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
		pos.z >= 0 && pos.z < m_Track->m_W;
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

STile CRouteTracker::getTile(const CTrack::CCheckpoint &pos) const
{
	unsigned int n = m_Track->m_H * (pos.z+m_Track->m_W*pos.x);

	vector<STile> candidates;
	for(int i=0; i < m_Track->m_H; i++)
	{
		STile &t = m_Track->m_Track[n+i];
		if(t.m_Model != 0 && getModel(t)->m_Routes.size() != 0)
			candidates.push_back(t);
	}

	//No candidates: return lowest tile
	if(candidates.size() == 0)
		return m_Track->m_Track[n];

	/*
	for(unsigned int i=0; i < candidates.size(); i++)
		printf("%d: %d\n", candidates[i].m_Z, candidates[i].m_Model);
	*/

	//Below lowest: return lowest
	if(pos.y < candidates[0].m_Z)
	{
		//printf("Below: %d < %d\n", pos.y, candidates[0].m_Z);
		return candidates[0];
	}

	for(unsigned int i=0; i < candidates.size(); i++)
	{
		//Return exact match:
		if(candidates[i].m_Z == pos.y)
		{
			//printf("Exact match\n");
			return candidates[i];
		}

		//TODO: between
	}

	//Above highest: return highest
	//printf("Above\n");
	return candidates[candidates.size()-1];
}

const CTETile *CRouteTracker::getModel(const STile &tile) const
{
	return (CTETile *)(m_DataManager->getObject(CDataObject::eTileModel, tile.m_Model));
}

