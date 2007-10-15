/***************************************************************************
                          mousecursor.h  -  A mouse cursor texture
                             -------------------
    begin                : zo aug 26 2007
    copyright            : (C) 2007 by CJP
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
#ifndef MOUSECURSOR_H
#define MOUSECURSOR_H

#include "texture.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CMouseCursor : protected CTexture
{
public:
	CMouseCursor();
	~CMouseCursor();

	void draw() const
		{CTexture::draw();}
};

#endif
