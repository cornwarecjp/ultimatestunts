/***************************************************************************
                          graphicobject.h  -  CGraphObj wrapper
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
#ifndef GRAPHICOBJECT_H
#define GRAPHICOBJECT_H

#include "graphobj.h"

/**
  *@author CJP
  */

class CGraphicObject
{
public:
	void setBody(CGraphObj *o){m_Body = o;}

	virtual void draw(){m_Body->draw();}
protected:

	CGraphObj *m_Body;
};

#endif
