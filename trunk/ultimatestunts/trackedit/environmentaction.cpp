/***************************************************************************
                          environmentaction.cpp  -  Changes the environment of a track
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
#include "environmentaction.h"

CEnvironmentAction::CEnvironmentAction(const CTrack::SEnv &e)
 : CAction()
{
	m_Environment = e;
}


CEnvironmentAction::~CEnvironmentAction()
{
}


bool CEnvironmentAction::doAction(CEditTrack* track) const
{
	track->m_Environment = m_Environment;
	return true;
}

