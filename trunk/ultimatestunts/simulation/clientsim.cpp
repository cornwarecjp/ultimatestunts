/***************************************************************************
                          clientsim.cpp  -  description
                             -------------------
    begin                : di jan 14 2003
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

#include "physics.h"
//#include "approximation.h"
#include "clientsim.h"

CClientSim::CClientSim(CWorld *w, CString HostName, int UDPPort) : CSimulation(w)
{
	//TODO: check connection speed

	//if(slownet)
		{m_SubSim = new CPhysics(w);}
	//else
	//	{m_SubSim = new CApproximation(w);}
}

CClientSim::~CClientSim()
{
	delete m_SubSim;
}

int CClientSim::addPlayer(CObjectChoice choice)
{
	//TODO: Register player via network and check if the player is accepted

	//if(accepted)
	{
		int id = CSimulation::addPlayer(choice);
		if(id < 0) //loading failed
			{;} //TODO: tell the server that loading failed

		return id;
	}

	//and in all other cases:
	return -1;
}

bool CClientSim::removePlayer(int id)
{
	bool unload = CSimulation::removePlayer(id);
	if(unload) //unloading was succesful
		{;} //TODO: tell the server to remove the player

	return unload;
}

void CClientSim::Update()
{
	//TODO: check network for new data, and update the world

	m_SubSim->Update();
}
