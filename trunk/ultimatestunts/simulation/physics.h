/***************************************************************************
                          physics.h  -  dummy file
                             -------------------
    begin                : Wed Nov 20 2002
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

#ifndef PHYSICS_H
#define PHYSICS_H


/**
  *@author CJP
  */
#include "simulation.h"

class CPhysics : public CSimulation {
public: 
	CPhysics();
	virtual ~CPhysics();

  //virtual bool addPlayer(CPlayer *p); //Using the default ones
  //virtual bool removePlayer(CPlayer *p);
};

#endif
