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
#include "generalmatrix.h"
#include "lconfig.h"

class CPhysics : public CSimulation {
public: 
	CPhysics(const CLConfig *conf);
	virtual ~CPhysics();

	virtual bool update();
protected:
	bool m_FastCPUMode;

	float m_dtMin;
	unsigned int m_NMax;
};

#endif
