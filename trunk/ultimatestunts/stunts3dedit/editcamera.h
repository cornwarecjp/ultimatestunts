/***************************************************************************
                          editcamera.h  -  description
                             -------------------
    begin                : wo apr 9 2003
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

#ifndef EDITCAMERA_H
#define EDITCAMERA_H

#include "camera.h"

/**
  *@author CJP
  */

class CEditCamera : public CCamera  {
public: 
	CEditCamera();
	~CEditCamera();

	void setDist(float dist);
	void setXAngle(float xangle);
	void setYAngle(float yangle);

	void incrDist(float dist);
	void incrXAngle(float xangle);
	void incrYAngle(float yangle);

	void flipCameraCenter();
protected:
	void updatePosition();

	float m_Dist, m_XAngle, m_YAngle;
	bool m_TileCenter;
};

#endif
