/***************************************************************************
                          dynamicobject.h  -  description
                             -------------------
    begin                : Thu Dec 5 2002
    copyright            : (C) 2002 by CJP
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

#ifndef DYNAMICOBJECT_H
#define DYNAMICOBJECT_H


/**
  *@author CJP
  */

#include "message.h"

class CDynamicObject : public CMessage {
public: 
	CDynamicObject();
	virtual ~CDynamicObject();

	CMessage *m_InputData;

	virtual bool hasChanged();
};

#endif
