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

#ifndef M_PI
#define M_PI 3.1415926536
#endif

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

		float t = m_Timer.getTime();

		if(t > theWorld->m_GameStartTime)
			theWorld->m_Paused = false;

		static int timedif = 4;

		int difnow = int(1.0 + theWorld->m_GameStartTime - t);
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
				updateCarRules((CCar *)theObj);
		}
	}

	return !checkFinished();
}

void CRuleControl::updateCarRules(CCar *car)
{
	float penaltyMultiplier = 3.0; //multiplier between fastest time and penalty time

	CCarRuleStatus &status = car->m_RuleStatus;
	CVector pos = car->m_Position;

	//integer position of the car
	int x = (int)(0.5 + (pos.x)/TILESIZE);
	int y = (int)(0.5 + (pos.y)/VERTSIZE);
	int z = (int)(0.5 + (pos.z)/TILESIZE);

	CTrack &track = *(theWorld->getTrack());
	unsigned int width = track.m_W;
	unsigned int height = track.m_H;

	//get the index of the tile around or below the car
	int base_index = height * z + height * width * x;
	unsigned int currentIndex = base_index;
	for(unsigned int i=0; i < height; i++)
	{
		STile &tile = track.m_Track[base_index+i];
		if(tile.m_Z > y || tile.m_Model == 0) break;
		currentIndex = base_index+i;
	}

	unsigned int prevIndex = status.currentTile;
	unsigned int validIndex = status.lastValidTile;

	int validCounter = track.m_Track[validIndex].m_RouteCounter,
		prevCounter = track.m_Track[prevIndex].m_RouteCounter,
		currentCounter = track.m_Track[currentIndex].m_RouteCounter;

	//are we inside the finish tile?
	bool currentIsFinish = theWorld->getTileModel(track.m_Track[currentIndex].m_Model)->m_isFinish;

	//Route tracking:
	if(currentIndex != prevIndex && currentCounter >= 0) //we're on a route, and not on the start
	{
		printf("\nvalid, prev, current = %d, %d, %d\n", validCounter, prevCounter, currentCounter);
		printf("curTileTime = %.2f s\n", track.m_Track[currentIndex].m_Time);
		printf("maxTileTime = %.2f s\n", status.maxTileTime);

		//special case for finish penalty time
		if(currentIsFinish)
		{
			float t1 = track.m_Track[validIndex].m_Time;
			float t2 = track.m_FinishTime;
			int finishCounter = track.m_FinishRouteCounter;

			if(finishCounter > validCounter+1 && t2-t1 > 0 && status.maxTileTime < t2)
				addPenaltytime(car, penaltyMultiplier * (t2-t1));
		}
		
		if(prevCounter>=0 && currentCounter == prevCounter+1) //we're following a certain route
		{
			float t1 = track.m_Track[validIndex].m_Time;
			float t2 = track.m_Track[prevIndex].m_Time;

			if(prevIndex != validIndex) //a gap in the route
			{
				if(t2-t1 > 0 && status.maxTileTime < t2)
					addPenaltytime(car, penaltyMultiplier * (t2-t1));
			}

			if(status.maxTileTime < t2) status.maxTileTime = t2;
			status.lastValidTile = currentIndex;
		}

		status.currentTile = currentIndex;
	}

	//Finish checking:
	//If we are inside the finish tile, and we have already done some part of the track:
	if(currentIsFinish && validCounter > 0)
	{
		const CBound *b = (CBound *)theWorld->getObject(CDataObject::eBound, car->m_Bodies[0].m_Body);
		float r = b->m_BSphere_r;

		CVector relpos = pos - CVector(TILESIZE*x, VERTSIZE*y, TILESIZE*z);

		float dist = TILESIZE; //default: too great distance
		switch(m_FinishRot)
		{
		case 0:
		case 2:
			dist = fabsf(relpos.z);
			break;
		case 1:
		case 3:
			dist = fabsf(relpos.x);
			break;
		}

		//if we have partially crossed the finish plane with the bounding sphere:
		if(dist < r)
			finish(car);
	}
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
		
		mo->resetBodyPositions(tilePos + tileOri * carPos, tileOri);

		CCarRuleStatus &status = ((CCar *)mo)->m_RuleStatus;
		status.lastValidTile = status.currentTile = m_StartIndex;
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

			if(status.state == CCarRuleStatus::eFinished &&
				m_Timer.getTime() - status.finishTime < 3.0)
			{
				ret = false;  //wait some time after everybody is finished
				break;
			}
		}
	}

	return ret;
}
