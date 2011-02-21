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

#include <GL/gl.h>

#include "texture.h"

/**
  *@author CJP
  */

class CFont : protected CTexture
{
public: 
	CFont(CDataManager *manager);
	virtual ~CFont();

	virtual bool load(const CString &filename, const CParamList &list);
	virtual void unload();

	//ALWAYS enable before drawing, and disable after (read the source)
	void enable();
	void disable();
	void drawString(const CString &str);

	float getFontW(){return m_W;}
	float getFontH(){return m_H;}

protected:
	float m_W, m_H;

	GLuint m_Texture;
	unsigned int m_BaseDispList;
};

#endif
