/***************************************************************************
                          serversim.h  -  Server-side networked simulation
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

#ifndef SERVERSIM_H
#define SERVERSIM_H

#include <vector>
#include "simulation.h"

/**
  *@author CJP
  */

class CServerSim : public CSimulation  {
public: 
	CServerSim(CWorld *w, int UDPPort);
	~CServerSim();

	virtual bool loadObjects();

	void addSubSim(CSimulation *s);

  virtual bool Update();

protected:
	vector<CSimulation *> m_SubSim;
};

#endif
