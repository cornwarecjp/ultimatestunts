/***************************************************************************
                          textureloader.h  -  description
                             -------------------
    begin                : do apr 10 2003
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

#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include "lodtexture.h"
#include "cstring.h"
#include "lconfig.h"

/**
  *@author CJP
  */

class CTextureLoader {
public: 
	CTextureLoader(const CLConfig &conf, CString texdatfile);
	~CTextureLoader();

	CLODTexture ** m_TexArray;
	int m_N;

protected:
	int m_TexMaxSize;
	bool m_TexSmooth;
};

#endif
