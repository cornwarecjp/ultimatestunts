/***************************************************************************
                          clientsim.h  -  description
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

#ifndef CLIENTSIM_H
#define CLIENTSIM_H

#include "simulation.h"

/**
  *@author CJP
  */

class CClientSim : public CSimulation  {
public: 
	CClientSim(CWorld *w, CString HostName, int UDPPort);
	~CClientSim();

  virtual int addPlayer(CObjectChoice choice);
  virtual bool removePlayer(int id);

  virtual void Update();

protected:
	CSimulation *m_SubSim;
};

#endif
