/***************************************************************************
                          serversim.cpp  -  description
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

int CServerSim::addPlayer(CObjectChoice choice)
{
	int id = CSimulation::addPlayer(choice);

	//TODO: send player info to clients

	return id;
}

bool CServerSim::removePlayer(int id)
{
	return false;
}

void CServerSim::addSubSim(CSimulation *s)
{
	m_SubSim.push_back(s);
}

void CServerSim::Update()
{
	//TODO: check for incoming data

	for(unsigned int i=0; i<m_SubSim.size(); i++)
		{(m_SubSim[i])->Update();}

	//TODO: send new data
}
