/***************************************************************************
                          dynamicobject.cpp  -  A dynamic object
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
#include "dynamicobject.h"


CDynamicObject::CDynamicObject()
{
//  m_Parameters = new CMessage;
//  m_InputData = new CMessage;
}

CDynamicObject::~CDynamicObject()
{
//  delete m_Parameters;
//  delete m_InputData;
}
/*

const CMessage *CDynamicObject::getParameters()
{
  return m_Parameters;
}


void CDynamicObject::setParameters(const CMessage *m)
{
  *m_Parameters = *m;
}

const CMessage *CDynamicObject::getInputData()
{
  return m_InputData;
}

void CDynamicObject::setInputData(const CMessage *m)
{
  *m_InputData = *m;
}

*/

bool CDynamicObject::hasChanged()
{return true;}
