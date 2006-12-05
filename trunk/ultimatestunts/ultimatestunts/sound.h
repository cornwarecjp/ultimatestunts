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
#include "usmacros.h"

#include "soundworld.h"
#include "music.h"
#include "world.h"

/**
  *@author CJP
  */

class CSound {
public: 
	CSound();
	~CSound();

	bool reloadConfiguration();

	void setCamera(const CCamera *cam)
		{m_Camera = (CGameCamera *)cam;}

	void playNextSong();

	bool load(); //loads samples, using the world object
	void unload(); //unloads the world samples

	void update();

protected:
	const CGameCamera *m_Camera;

	CMusic *m_Music;
	CSoundObj *m_MusicObject;
	vector<CString> m_Playlist;
	unsigned int m_PlaylistItem;

	CSoundWorld *m_SoundWorld;

	int m_MusicVolume, m_SoundVolume;
};

#endif
