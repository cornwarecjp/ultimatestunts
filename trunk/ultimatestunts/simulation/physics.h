/***************************************************************************
                          physics.h  -  The physics of the game :-)
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
#include "timer.h"

class CPhysics : public CSimulation {
public: 
	CPhysics(CWorld *w);
	virtual ~CPhysics();

  //virtual bool addPlayer(CPlayer *p, CObjectChoice choice); //Using the default ones
  //virtual bool removePlayer(CPlayer *p);

  virtual void Update();

protected:
	CTimer m_Timer;
};

#endif
