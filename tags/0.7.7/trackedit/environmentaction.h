/***************************************************************************
                          environmentaction.h  -  Changes the environment of a track
                             -------------------
    begin                : wo dec 22 2010
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
#ifndef ENVIRONMENTACTION_H
#define ENVIRONMENTACTION_H

#include "action.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CEnvironmentAction : public CAction
{
public:
	CEnvironmentAction(const CTrack::SEnv &e);
	~CEnvironmentAction();
	
	virtual bool doAction(CEditTrack* track) const;

	virtual CAction* copy() const
		{return new CEnvironmentAction(*this);}

	CTrack::SEnv m_Environment;
};

#endif
