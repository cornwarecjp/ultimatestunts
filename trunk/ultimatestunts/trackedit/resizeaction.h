/***************************************************************************
                          resizeaction.h  -  Resizes the track
                             -------------------
    begin                : di dec 28 2010
    copyright            : (C) 2010 by CJP
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
#ifndef RESIZEACTION_H
#define RESIZEACTION_H

#include "action.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CResizeAction : public CAction
{
public:
	CResizeAction(int dxmin, int dxmax, int dzmin, int dzmax);
	~CResizeAction();
	
	virtual bool doAction(CEditTrack* track) const;

	virtual CAction* copy() const
		{return new CResizeAction(*this);}

	int m_dxmin;
	int m_dxmax;
	int m_dzmin;
	int m_dzmax;
};

#endif
