/***************************************************************************
                          background.h  -  description
                             -------------------
    begin                : di feb 4 2003
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

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "texture.h"
#include "timer.h"

/**
  *@author CJP
  */

class CBackground : public CTexture
{
public: 
	CBackground(CDataManager *manager);
	virtual ~CBackground();

	void draw() const;
protected:
	CTimer m_Timer;
};

#endif
