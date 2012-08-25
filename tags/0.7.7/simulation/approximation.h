/***************************************************************************
                          approximation.h  -  Approximate physics behavior between network updates
                             -------------------
    begin                : do dec 15 2005
    copyright            : (C) 2005 by CJP
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

#ifndef APPROXIMATION_H
#define APPROXIMATION_H

#include "simulation.h"

/**
  *@author CJP
  */

class CApproximation : public CSimulation  {
public: 
	CApproximation();
	virtual ~CApproximation();

	virtual bool update();
};

#endif
