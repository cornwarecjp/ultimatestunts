/***************************************************************************
                          soundworld.h  -  All the sound data
                             -------------------
    begin                : ma aug 11 2003
    copyright            : (C) 2003 by CJP
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

#ifndef SOUNDWORLD_H
#define SOUNDWORLD_H

#include <vector>
namespace std {}
using namespace std;
#include "lconfig.h"

#include "sndsample.h"
#include "soundobj.h"
#include "world.h"
#include "datamanager.h"

/**
  *@author CJP
  */

class CSoundWorld : public CDataManager
{
public: 
	CSoundWorld(const CLConfig &conf);
	~CSoundWorld();

	const CWorld *m_World;

	bool loadObjects();
	void unloadObjects();

	vector<CSoundObj *> m_Channels;

protected:
	virtual CDataObject *createObject(const CString &filename, const CParamList &plist, CDataObject::eDataType type);

};

#endif
