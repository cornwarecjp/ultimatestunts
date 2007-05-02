/***************************************************************************
                          tecamera.h  -  The camera of the track editor
                             -------------------
    begin                : ma mei 23 2005
    copyright            : (C) 2005 by CJP
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

#ifndef TECAMERA_H
#define TECAMERA_H

#include "camera.h"

/**
  *@author CJP
  */

class CTECamera : public CCamera  {
public: 
	CTECamera();
	~CTECamera();

	void turnRight(float angle);
	void turnUp(float angle);
	void zoomOut(float factor);

	void setTargetPos(CVector p);

protected:
	void updatePosition();

	CVector m_TargetPos;
	float m_XAngle, m_YAngle, m_Distance;
};

#endif
