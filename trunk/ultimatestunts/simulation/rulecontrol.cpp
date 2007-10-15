/***************************************************************************
                          rulecontrol.cpp  -  Check if players follow the rules
                             -------------------
    begin                : do sep 11 2003
    copyright            : (C) 2003 by CJP
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

#include <cstdio> //debugging
#include <cmath>

#include "pi.h"

#include <libintl.h>
#define _(String) gettext (String)
#define N_(String1, String2, n) ngettext ((String1), (String2), (n))

#include "rulecontrol.h"
#include "bound.h"
#include "usmacros.h"

CRuleControl::CRuleControl()
{
	firstUpdate = true;
}

CRuleControl::~CRuleControl(){
}

bool CRuleControl::update()
{
	//Place everybody at the start (only the first update of course)
	if(firstUpdate)
	{
		firstUpdate = false;

		if(!findStartFinish()) //error with starts/finishes
			return false; //stop

		placeStart();
		theWorld->m_Paused = true;
	}

	if(theWorld->m_Paused)
	{
		//Pause state before the start of the game

		float t = theWorld->m_LastTime;

		if(t > 0.0)
		{
			theWorld->m_Paused = false;
			placeStart(); //sets start times
		}

		static int timedif = 4;

		int difnow = int(1.0 - t);
		if(difnow != timedif)
		{
			timedif = difnow;

			CChatMessage msg;
			if(timedif > 0)
				{msg.m_Message = CString(timedif);}
			else
				{msg.m_Message = _("GO!");}

			msg.m_ToMovingObject = -1;
			theWorld->m_ChatSystem.sendMessage(msg);
		}
	}
	else
	{
		//Normal racing rule update
		vector<CDataObject *> objs = theWorld->getObjectArray(CDataObject::eMovingObject);
		for(unsigned int i=0; i < objs.size(); i++)
		{
			CMovingObject *theObj = (CMovingObject *)objs[i];
			if(theObj->getType() == CMessageBuffer::car)
				updateCarRules(i, (CCar *)theObj);

			m_PreviousCarPositions[i] = theObj->m_Position;
		}
	}

	return !checkFinished();
}

void CRuleControl::updateCarRules(unsigned int movObjIndex, CCar *car)
{
	//float penaltyMultiplier = 3.0; //multiplier between fastest time and penalty time

	CCarRuleStatus &status = car->m_RuleStatus;
	CVector pos = car->m_Position;

	//integer position of the car
	int x = (int)(0.5 + pos.x/TILESIZE);
	int y = (int)(pos.y/VERTSIZE); if(pos.y < 0) y--;
	int z = (int)(0.5 + pos.z/TILESIZE);
	CTrack::CCheckpoint carpos;
	carpos.x = x;
	carpos.y = y;
	carpos.z = z;

	CTrack &track = *(theWorld->getTrack());

	//Find it on the current position
	int routenr = -1, tilenr = -1;
	if(track.m_Routes[status.currentRoute][status.currentTile] == carpos)
	{
		routenr = (int)status.currentRoute; tilenr = (int)status.currentTile;
	}

	//Find it on the current route ahead
	if(routenr < 0 && (status.currentTile+1 < track.m_Routes[status.currentRoute].size()) )
		for(int i=status.currentTile+1; i < (int)track.m_Routes[status.currentRoute].size() ; i++)
			if(track.m_Routes[status.currentRoute][i] == carpos)
			{
				routenr = (int)status.currentRoute; tilenr = i;
				break;
			}

	//Find it on the current route behind
	if(routenr < 0 && (status.currentTile > 0) )
		for(int i=status.currentTile-1; i >= 0 ; i--)
			if(track.m_Routes[status.currentRoute][i] == carpos)
			{
				routenr = (int)status.currentRoute; tilenr = i;
				break;
			}

	//Find it on other routes
	//Prefer routes which are completed until this tile
	//Even better: routes which also contain the previous tile
	//Prefer lower route-numbers
	unsigned int prefLevel = 0; //0 = nothing, 1 = completed 2 = contains previous
	if(routenr < 0)
	{
		for(int r=track.m_Routes.size()-1; r >= 0 ; r--)
			for(int i=track.m_Routes[r].size()-1; i >= 0 ; i--)
				if(track.m_Routes[r][i] == carpos)
				{
					unsigned int thisLevel = 0;

					if(i <= status.lastPosition[r]+1)
						thisLevel = 1;

					if(i > 0 &&
						track.m_Routes[r][i-1] ==
						track.m_Routes[status.currentRoute][status.currentTile]
						)
						thisLevel = 2;

					if(thisLevel >= prefLevel)
					{
						routenr = r; tilenr = i;
						prefLevel = thisLevel;
					}
				}
	}

	bool doFinishCheck = false;
	if(routenr == 0 && tilenr == (int)(track.m_Routes[0].size()-1))
	{
		//printf("Enabling finish\n");
		doFinishCheck = true;
	}

	//Check whether we are on a new route tile
	if(routenr >= 0 && (routenr != (int)status.currentRoute || tilenr != (int)status.currentTile) )
	{
		//printf("We are on route %d tile %d\n", routenr, tilenr);

		CTrack::CRoute &theRoute = track.m_Routes[routenr];

		if(tilenr == status.lastPosition[routenr]+1) //exploration of this route
		{
			//printf("Normal following route %d\n", routenr);
			status.lastPosition[routenr] = tilenr;

			if(routenr > 0 &&
				status.lastPosition[routenr] ==
					(int)(track.m_Routes[routenr].size())-1) //end of sub-route
			{
				//printf("End of route %d, continuing route %d\n", routenr, theRoute.finishRoute);
				tilenr = theRoute.finishTile;
				routenr = theRoute.finishRoute;

				//Also finished this part of the route:
				status.lastPosition[routenr] = tilenr;
			}
		}
		else if(tilenr <= status.lastPosition[routenr]) //we've already been there
		{
			//printf("Already been here\n");
			; //nothing to do
		}
		else if(tilenr == 1 && status.lastPosition[theRoute.startRoute] >= (int)(theRoute.startTile))
		{
			//new sub-route

			//printf("Entered route %d\n", routenr);
			status.lastPosition[routenr] = tilenr;
		}
		else if(
			(routenr == (int)(status.currentRoute) && tilenr == (int)(status.currentTile+1))
				||
			doFinishCheck
			)
		{
			//Following the wrong route, not yet been there: must give penalty points

			//Calculate minimum distance following a valid route
			SRoutePos from, to;
			if(doFinishCheck)
			{
				to.route = routenr;
				to.tile = tilenr;
			}
			else
			{
				to.route = status.currentRoute;
				to.tile = status.currentTile;
			}
			float minDistance = getSmallestDistance(status, from, to); //fills in from

			//Calculate minimum distance following a straight line
			printf("Searching path to route %d tile %d\n", to.route, to.tile);
			float lineDistance = getDirectDistance(from, to);

			printf("Valid distance: %.1f; Line distance: %.1f\n", minDistance, lineDistance);

			float dx = minDistance - lineDistance;
			if(dx > 0) //expected to be always true, but you never know
			{
				float dt = dx / 10.0; //10 m/s
				if(dt >= 0.500) //threshold: not too small penalty times
					addPenaltytime(car, dt);
			}

			reachRouteRecursive(status, routenr, tilenr);
		}
		else
		{
			//printf("You are not supposed to be on route %d tile %d\n", routenr, tilenr);
		}

		if(status.lastPosition[routenr] >= tilenr) //we are legally on this position
		{
			//Enter possible alternative routes
			for(unsigned int r=0; r<track.m_Routes.size(); r++)
				if(track.m_Routes[r][0] == track.m_Routes[routenr][tilenr]) //we are on the start tile of the route
				{
					//printf("Allowing alternative route %d\n", r);
					if(status.lastPosition[r] < 0)
						status.lastPosition[r] = 0;
				}
		}

		status.currentRoute = routenr;
		status.currentTile = tilenr;
	}

	/*
	if(routenr < 0)
	{
		printf("Position %d, %d, %d is not on any route\n", x, y, z);
	}
	*/

	//Finish checking:
	//If we are inside the finish tile, and we have already done some part of the track:
	if(doFinishCheck) //currentIsFinish && racedSomeTrack)
	{
		const CBound *b = (CBound *)theWorld->getObject(CDataObject::eBound, car->m_Bodies[0].m_Body);
		float r = b->m_BSphere_r;

		CVector relpos1 = m_PreviousCarPositions[movObjIndex] - CVector(TILESIZE*x, VERTSIZE*y, TILESIZE*z);
		CVector relpos2 = pos - CVector(TILESIZE*x, VERTSIZE*y, TILESIZE*z);

		float dist1 = TILESIZE, dist2 = TILESIZE; //default: too great distance
		switch(m_FinishRot)
		{
		case 0:
		case 2:
			dist1 = relpos1.z;
			dist2 = relpos2.z;
			break;
		case 1:
		case 3:
			dist1 = relpos1.x;
			dist2 = relpos2.x;
			break;
		}

		//Finish...
		//...if we have partially crossed the finish plane with the bounding sphere
		//...if we are on the other side of the finish plane
		if(fabsf(dist2) < r || dist1 / dist2 < 0.0)
			finish(car);
	}

}

void CRuleControl::reachRouteRecursive(CCarRuleStatus &status, unsigned int route, unsigned int tile)
{
	if(status.lastPosition[route] < 0)
	{
		CTrack &track = *(theWorld->getTrack());
		CTrack::CRoute &r = track.m_Routes[route];
		reachRouteRecursive(status, r.startRoute, r.startTile);
	}

	if(status.lastPosition[route] < (int)tile)
	{
		status.lastPosition[route] = tile;
	}
}

float CRuleControl::getSmallestDistance(const CCarRuleStatus &status, SRoutePos &from, SRoutePos to)
{
	//End of recursion situation:
	if(status.lastPosition[to.route] >= (int)(to.tile))
	{
		//printf("From route %d tile %d\n", to.route, to.tile);
		from = to;
		return 0.0;
	}

	float ret = -1.0;

	CTrack &track = *(theWorld->getTrack());

	if(to.tile > 0)
	{
		SRoutePos mid = to;
		mid.tile--;
		ret = getSmallestDistance(status, from, mid) + getDirectDistance(mid, to);
	}
	else //to.tile == 0, follow a split upstream
	{
		SRoutePos to2;
		to2.route = track.m_Routes[to.route].startRoute;
		to2.tile = track.m_Routes[to.route].startTile;
		ret = getSmallestDistance(status, from, to2);
	}

	//Follow joins upstream
	for(unsigned int i=0; i < track.m_Routes.size(); i++)
		if(track.m_Routes[i].finishRoute == to.route && track.m_Routes[i].finishTile == to.tile)
		{
			SRoutePos to2, from2;
			to2.route = i;
			to2.tile = track.m_Routes[i].size()-1;
			float ret2 = getSmallestDistance(status, from2, to2);

			if(ret2 < ret)
			{
				ret = ret2;
				from = from2;
			}
		}

	//printf("  Via route %d tile %d\n", to.route, to.tile);

	return ret;
}

float CRuleControl::getDirectDistance(SRoutePos from, SRoutePos to)
{
	CTrack &track = *(theWorld->getTrack());

	CTrack::CCheckpoint p1, p2;
	p1 = track.m_Routes[from.route][from.tile];
	p2 = track.m_Routes[to.route][to.tile];

	float dx = TILESIZE * ((float)p1.x - (float)p2.x);
	float dy = VERTSIZE * ((float)p1.y - (float)p2.y);
	float dz = TILESIZE * ((float)p1.z - (float)p2.z);

	return sqrt(dx*dx + dy*dy + dz*dz);
}

void CRuleControl::addPenaltytime(CCar *car, float t)
{
	if(car->m_RuleStatus.addPenalty(t))
	{
		CChatMessage m;
		m.m_Message = CString(_("Penalty time: ")) + (CString().fromTime(t));
		m.m_ToMovingObject = car->getMovObjID();
		theWorld->m_ChatSystem.sendMessage(m);
	}
}

void CRuleControl::finish(CCar *car)
{
	if(car->m_RuleStatus.finish())
	{
		CChatMessage m;
		m.m_Message = CString(_("You finished (waiting for other players)"));
		m.m_ToMovingObject = car->getMovObjID();
		theWorld->m_ChatSystem.sendMessage(m);
	}
}

bool CRuleControl::findStartFinish()
{
	bool founds = false, foundf = false;

	CTrack &track = *(theWorld->getTrack());
	int wth = track.m_W;
	int hth = track.m_H;
	for(int x = 0; x < track.m_L; x++)
		for(int y = 0; y < wth; y++)
			for(int h = 0; h < hth; h++)
			{
				int index = h + hth*(y + wth*x);
				STile &tile = track.m_Track[index];
				
				if(theWorld->getTileModel(tile.m_Model)->m_isStart)

					if(founds) //more than 1 start position
						{printf(_("Error: more than one start in the track\n")); return false;}
					else
					{
						founds = true;
						m_StartX = x; m_StartY = y; m_StartH = tile.m_Z; m_StartRot = tile.m_R;
						m_StartIndex = index;
					}

				if(theWorld->getTileModel(tile.m_Model)->m_isFinish)

					if(foundf) //more than 1 finish position
						{printf(_("Error: more than one finish in the track\n")); return false;}
					else
					{
						foundf = true;
						m_FinishX = x; m_FinishY = y; m_FinishH = tile.m_Z; m_FinishRot = tile.m_R;
						m_FinishIndex = index;
					}
			}

	if(!founds || !foundf) //no start or finish position
		return false;

	return true;
}

void CRuleControl::placeStart()
{
	m_PreviousCarPositions.clear();

	CVector tilePos = CVector(
		TILESIZE * m_StartX,
		VERTSIZE * m_StartH,
		TILESIZE * m_StartY
	);

	CMatrix tileOri;
	tileOri.rotY(m_StartRot * -0.5 * M_PI);

	/*
	Starting positions:
	*---*
	|0 1|
	|2 3|
	|4 5|
	|etc|
	*---*
	*/
	for(unsigned int i=0; i<theWorld->getNumObjects(CDataObject::eMovingObject); i++)
	{
		CVector carPos = CVector(
			-3.0*(i%2==0) +3.0*(i%2!=0),
			0.4,
			6.0*(i-i%2) + 3.0
		);

		CMovingObject *mo = theWorld->getMovingObject(i);

		CVector position = tilePos + tileOri * carPos;
		mo->m_Position = position;
		mo->m_OrientationMatrix = tileOri;
		mo->resetBodyPositions();
		m_PreviousCarPositions.push_back(position);

		CCarRuleStatus &status = ((CCar *)mo)->m_RuleStatus;
		//status.lastValidTile = status.currentTile = m_StartIndex;
		status.start();
	}
}

bool CRuleControl::checkFinished()
{
	bool ret = true; //default: assume that everybody wants to stop
	
	vector<CDataObject *> objs = theWorld->getObjectArray(CDataObject::eMovingObject);
	for(unsigned int i=0; i < objs.size(); i++)
	{
		CMovingObject *theObj = (CMovingObject *)objs[i];
		if(theObj->getType() == CMessageBuffer::car)
		{
			CCarRuleStatus &status = ((CCar *)theObj)->m_RuleStatus;
			if(status.state == CCarRuleStatus::eNotStarted || status.state == CCarRuleStatus::eRacing)
			{
				ret = false;  //some want to race -> continue
				break;
			}

			if(
				(status.state == CCarRuleStatus::eFinished ||
				 status.state == CCarRuleStatus::eCrashed)
					&&
				theWorld->m_LastTime - status.finishTime < 3.0)
			{
				ret = false;  //wait some time after everybody is finished or crashed
				break;
			}
		}
	}

	return ret;
}
