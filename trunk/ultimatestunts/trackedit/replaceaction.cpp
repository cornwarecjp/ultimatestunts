/***************************************************************************
                          replaceaction.cpp  -  Replace tiles with other tiles
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

#include "replaceaction.h"
#include "trackdocument.h"

CReplaceAction::CReplaceAction()
{
	m_ClearTile = true;
}

CReplaceAction::~CReplaceAction()
{;}

bool CReplaceAction::doAction(CEditTrack *track) const
{
	STile newTile = m_Tile;
	newTile.m_Z = theTrackDocument->getCursorY();

	if(m_Tile.m_Model >= int(
		theTrackDocument->m_DataManager->getNumObjects(CDataObject::eTileModel)
		))
			newTile.m_Model = 1;

	//Do some action
	unsigned int pilindex = track->m_H *
		(track->m_W * theTrackDocument->getCursorX() + theTrackDocument->getCursorZ());

	if(newTile.m_Model == 0) //delete everything on the tile height
	{
		for(int h=0; h < track->m_H; h++)
		{
			if(track->m_Track[pilindex + h].m_Model == 0) break;

			while(track->m_Track[pilindex + h].m_Z == newTile.m_Z)
			{
				removeTile(track, pilindex, h);
				if(track->m_Track[pilindex + h].m_Model == 0) break;
			}
		}

		return true;
	}

	for(int h=0; h < track->m_H; h++)
	{
		STile &tile = track->m_Track[pilindex + h];

		if(tile.m_Model == 0)
		{
			tile = newTile;
			return true;
		}
		else if(tile.m_Z == newTile.m_Z)
		{
			if(m_ClearTile)
			{
				tile = newTile;

				//Remove everything else on this height
				if(h < track->m_H-1)
					while(track->m_Track[pilindex + h+1].m_Z == tile.m_Z)
						removeTile(track, pilindex, h+1);
			}
			else
			{
				//If full, increase the track height and try again:
				if(track->m_Track[pilindex + track->m_H-1].m_Model != 0)
				{
					track->setHeight(track->m_H + 1);
					return doAction(track);
				}

				//Determine lowest "other" tile
				int h2=h;
				for(; h2 < track->m_H; h2++)
				{
					if(track->m_Track[pilindex + h2].m_Z > newTile.m_Z) break;
					if(track->m_Track[pilindex + h2].m_Model == 0) break;
				}

				//Insert there
				insertTile(track, pilindex, h2, newTile);

			}

			return true;
		}
		else if(tile.m_Z > newTile.m_Z)
		{
			//If full, increase the track height and try again:
			if(track->m_Track[pilindex + track->m_H-1].m_Model != 0)
			{
				track->setHeight(track->m_H + 1);
				return doAction(track);
			}

			insertTile(track, pilindex, h, newTile);
			return true;
		}
	}

	//Full: increase the track height and try again
	track->setHeight(track->m_H + 1);
	return doAction(track);
}

void CReplaceAction::insertTile(CEditTrack *track, unsigned int offset, int h, STile &tile) const
{
	//Move up
	for(int h2=track->m_H-1; h2 > h; h2--)
		track->m_Track[offset + h2] = track->m_Track[offset + h2-1];

	track->m_Track[offset + h] = tile;
}

void CReplaceAction::removeTile(CEditTrack *track, unsigned int offset, int h) const
{
	//Move down
	for(int h2=h; h2 < track->m_H-1; h2++)
		track->m_Track[offset + h2] = track->m_Track[offset + h2+1];

	STile empty;
	empty.m_Model = 0;
	empty.m_R = 0;
	empty.m_Z = track->m_Track[offset + track->m_H-1].m_Z+1;
	if(track->m_Track[offset + track->m_H-1].m_Model != 0) empty.m_Z += 9;

	track->m_Track[offset + track->m_H-1] = empty;
}
