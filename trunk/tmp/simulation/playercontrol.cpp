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
		ret = loadOneObject(m_LocalChoices[i], i) && ret;

	return ret;
}

bool CPlayerControl::loadOneObject(CObjectChoice &oc, unsigned int ID)
{
	CParamList plist = oc.m_Parameters;
	SParameter p;
	p.name = "ID";
	p.value = (int)ID;
	plist.push_back(p);
	return (theWorld->loadObject(oc.m_Filename, plist, CDataObject::eMovingObject) >= 0);
}

void CPlayerControl::clearPlayerList()
{
	m_LocalChoices.clear();
}
