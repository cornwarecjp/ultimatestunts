/***************************************************************************
                          gamecamera.h  -  The camera being used in the game
                             -------------------
    begin                : ma feb 3 2003
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

#ifndef GAMECAMERA_H
#define GAMECAMERA_H

#include "camera.h"
#include "world.h"

/**
  *@author CJP
  */

class CGameCamera : public CCamera  {
public: 
	enum eCameraMode {
		In=1,
		Tracking,
		UserDefined,
		Top,
		Television
	};

	CGameCamera(const CWorld *w);
	virtual ~CGameCamera();

	void setCameraMode(eCameraMode mode);
	void setTrackedObject(int id);

	virtual void update();
protected:
	eCameraMode m_Mode;
	int m_Id;
	const CWorld *m_World;

	bool m_Reached;
};

#endif
