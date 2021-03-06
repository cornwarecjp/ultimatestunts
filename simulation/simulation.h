/***************************************************************************
                          simulation.h  -  Base-class for simulations
                             -------------------
    begin                : Wed Dec 4 2002
    copyright            : (C) 2002 by CJP
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

#ifndef SIMULATION_H
#define SIMULATION_H


/**
  *@author CJP
  */

#include "world.h"

class CSimulation {
public: 
	CSimulation();
	virtual ~CSimulation();

	//while playing:
	virtual bool update() =0;
};

#endif
