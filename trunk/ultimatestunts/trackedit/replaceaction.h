/***************************************************************************
                          replaceaction.h  -  Replace tiles with other tiles
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

#ifndef REPLACEACTION_H
#define REPLACEACTION_H

#include "action.h"

/**
  *@author CJP
  */

class CReplaceAction : public CAction
{
public:
	CReplaceAction();
	virtual ~CReplaceAction();

	virtual bool doAction(CEditTrack *track) const;

	virtual CAction *copy() const
		{return new CReplaceAction(*this);}

	bool m_ClearTile;
	STile m_Tile;

protected:
	void insertTile(CEditTrack *track, unsigned int offset, int h, STile &tile) const;
	void removeTile(CEditTrack *track, unsigned int offset, int h) const;
};

#endif
