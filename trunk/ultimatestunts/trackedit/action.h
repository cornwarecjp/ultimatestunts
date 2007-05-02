/***************************************************************************
                          action.h  -  An action that can be applied to a track
                             -------------------
    begin                : vr dec 15 2006
    copyright            : (C) 2006 by CJP
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

#ifndef ACTION_H
#define ACTION_H

#include "edittrack.h"

/**
  *@author CJP
  */

class CAction
{
public:
	CAction(){;}
	virtual ~CAction(){;}

	virtual bool doAction(CEditTrack *track) const =0;
};

#endif
