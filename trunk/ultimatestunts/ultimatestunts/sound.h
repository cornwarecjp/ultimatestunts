/***************************************************************************
                          sound.h  -  The basic sound system
                             -------------------
    begin                : di feb 25 2003
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

#ifndef SOUND_H
#define SOUND_H

#include <vector>
namespace std {}
using namespace std;

#include "gamecamera.h"
#include "lconfig.h"

#include "sndsample.h"
#include "soundobj.h"

/**
  *@author CJP
  */

class CSound {
public: 
	CSound(const CLConfig &conf, const CWorld *world);
	~CSound();

	void setCamera(const CCamera *cam)
		{m_Camera = (CGameCamera *)cam;}

	bool load(); //loads samples, using the world object
	void unload(); //unloads the world samples

	void update();

protected:
	const CGameCamera *m_Camera;
	const CWorld *m_World;

	CSndSample *m_MusicSample;
	CSoundObj *m_MusicObject;

	vector<CSoundObj *> m_Channels;
	vector<int> m_ObjIDs;

	vector<CSndSample *> m_Samples;

	int m_MusicVolume, m_SoundVolume;
};

#endif
