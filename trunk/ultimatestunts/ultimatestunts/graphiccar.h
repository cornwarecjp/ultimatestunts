/***************************************************************************
                          graphiccar.h  -  A graphical version of a car object
                             -------------------
    begin                : do jan 16 2003
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

#ifndef GRAPHICCAR_H
#define GRAPHICCAR_H

#include "graphobj.h"
#include "car.h"
#include "graphicobject.h"

/**
  *@author CJP
  */

class CGraphicCar : public CCar, public CGraphicObject
{
public: 
	CGraphicCar();
	virtual ~CGraphicCar();

	//No getType overload: in its messaging behaviour
	//CGraphicCar is just a normal CCar

	//Other parts of the car:
	void setFrontWheel(CGraphObj *o){m_FrontWheel = o;}
	void setBackWheel(CGraphObj *o){m_BackWheel = o;}

	virtual void draw();
protected:

	CGraphObj *m_FrontWheel;
	CGraphObj *m_BackWheel;
};

#endif
