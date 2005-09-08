/***************************************************************************
                          edittrack.h  -  An editable track
                             -------------------
    begin                : ma mei 23 2005
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

#ifndef EDITTRACK_H
#define EDITTRACK_H

#include "track.h"

/**
  *@author CJP
  */

class CEditTrack : public CTrack  {
public: 
	CEditTrack(CDataManager *manager);
	~CEditTrack();
};

#endif
