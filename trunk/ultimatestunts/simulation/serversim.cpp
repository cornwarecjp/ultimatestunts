/***************************************************************************
                          serversim.cpp  -  Server-side networked simulation
                             -------------------
    begin                : wo jan 15 2003
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

#include "serversim.h"

CServerSim::CServerSim(CWorld *w, int UDPPort) : CSimulation(w)
{
}

CServerSim::~CServerSim()
{
}

bool CServerSim::loadObjects()
{
	//TODO: send clients the player-information

	return CSimulation::loadObjects();
}

void CServerSim::addSubSim(CSimulation *s)
{
	m_SubSim.push_back(s);
}

bool CServerSim::Update()
{
	bool cont_game = true;

	//TODO: check for incoming data

	for(unsigned int i=0; i<m_SubSim.size(); i++)
		{cont_game = (m_SubSim[i])->Update() && cont_game;}

	//TODO: send new data

	return cont_game;
}
