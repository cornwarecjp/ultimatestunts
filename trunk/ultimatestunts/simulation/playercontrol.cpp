/***************************************************************************
                          playercontrol.cpp  -  Manage the players
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

#include "playercontrol.h"

CPlayerControl::CPlayerControl()
{
}

CPlayerControl::~CPlayerControl()
{
}

int CPlayerControl::addPlayer(CObjectChoice choice)
{
	//Default behaviour: always accept a player
	m_LocalChoices.push_back(choice);
	return m_LocalChoices.size()-1;
}

bool CPlayerControl::loadObjects()
{
	bool ret = true;
	for(unsigned int i=0; i < m_LocalChoices.size(); i++)
	{
		//every player's vehicle is a unique object, even if
		//they are loaded from the same file. That's why we
		//have to add a unique parameter for each moving object:
		CParamList plist;
		SParameter p;
		p.name = "ID";
		p.value = (int)i;
		plist.push_back(p);
		ret = (theWorld->loadObject(m_LocalChoices[i].m_Filename, plist, CDataObject::eMovingObject) >= 0) && ret;
	}

	return ret;
}

void CPlayerControl::clearPlayerList()
{
	m_LocalChoices.clear();
}
