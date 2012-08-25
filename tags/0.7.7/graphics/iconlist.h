/***************************************************************************
                          iconlist.h  -  List of icons for use in GUIs
                             -------------------
    begin                : za jun 9 2007
    copyright            : (C) 2007 by CJP
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
#ifndef ICONLIST_H
#define ICONLIST_H

#include "texture.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CIconList : public CTexture
{
public:
	CIconList();
	~CIconList();

	enum eIconID {
		eLoad=0, eSave=1, eUndo=2, eRedo=3, eQuit=4, eHelp=5, eImport=6, eTiles=7,
		eRotateTile=8, eCombineTiles=9, eOK = 10, eDelete=11, eScenery=12,
		eResizeTrack=13,
		eNone
		};

	void enable();
	void disable();

	void draw(eIconID ID);
};

extern CIconList *theIconList;

#endif
