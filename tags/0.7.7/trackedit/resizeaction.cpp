/***************************************************************************
                          resizeaction.cpp  -  Resizes the track
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
#include "resizeaction.h"

CResizeAction::CResizeAction(int dxmin, int dxmax, int dzmin, int dzmax)
{
	m_dxmin = dxmin;
	m_dxmax = dxmax;
	m_dzmin = dzmin;
	m_dzmax = dzmax;
}


CResizeAction::~CResizeAction()
{
}


bool CResizeAction::doAction(CEditTrack* track) const
{
	if(m_dxmin + m_dxmax + track->m_L <= 0) return false;
	if(m_dzmin + m_dzmax + track->m_W <= 0) return false;

	vector<STile>  oldTrack = track->m_Track;
	vector<STile> &newTrack = track->m_Track;
	int oldL = track->m_L;
	int oldW = track->m_W;

	track->m_L = m_dxmin + m_dxmax + oldL;
	track->m_W = m_dzmin + m_dzmax + oldW;
	newTrack.resize(track->m_L * track->m_W * track->m_H);

	//Initialize empty:
	STile empty;
	empty.m_Model = 0;
	empty.m_Z = 0;
	empty.m_R = 0;
	for(unsigned int i=0; i < newTrack.size(); i++)
		newTrack[i] = empty;

	//Copy from the old track:
	for(int x=0; x < oldL; x++)
	{
		if(x + m_dxmin < 0) continue;
		if(x + m_dxmin >= track->m_L) break;

		for(int z=0; z < oldW; z++)
		{
			if(z + m_dzmin < 0) continue;
			if(z + m_dzmin >= track->m_W) break;

			int oldOffset = track->m_H * (oldW*x + z);
			int newOffset = track->m_H * (track->m_W*(x+m_dxmin) + (z+m_dzmin));

			for(int y=0; y < track->m_H; y++)
				newTrack[newOffset+y] = oldTrack[oldOffset+y];
		}
	}

	return true;
}

