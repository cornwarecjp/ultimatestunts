/***************************************************************************
                          font.h  -  A bitmap-based font class
                             -------------------
    begin                : ma okt 25 2004
    copyright            : (C) 2004 by CJP
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

#ifndef FONT_H
#define FONT_H

#include "texture.h"

/**
  *@author CJP
  */

class CFont : protected CTexture
{
public: 
	CFont();
	virtual ~CFont();

	bool loadFromFile(const CString &file, unsigned int texsize, float wth, float hth);
	void unload();

	//ALWAYS enable before drawing, and disable after (read the source)
	void enable();
	void disable();
	void drawString(const CString &str);

protected:
	unsigned int m_Texture;
	unsigned int m_BaseDispList;
};

#endif
