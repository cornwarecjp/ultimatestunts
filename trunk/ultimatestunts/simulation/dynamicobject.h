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

class CDynamicObject {
public: 
	CDynamicObject();
	virtual ~CDynamicObject();

//  virtual const CMessage *getParameters();
//  virtual void setParameters(const CMessage *m);

//  virtual const CMessage *getInputData(); //Used by network
//  virtual void setInputData(const CMessage *m);

  virtual bool hasChanged();

protected:
//  CMessage *m_Parameters;
//  CMessage *m_InputData;
};

#endif
