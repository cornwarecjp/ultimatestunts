/***************************************************************************
                          graphicmaterial.h  -  A material and a texture
                             -------------------
    begin                : ma jan 27 2003
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
#ifndef GRAPHICMATERIAL_H
#define GRAPHICMATERIAL_H

#include "material.h"
#include "texture.h"

class CGraphicMaterial : public CMaterial, public CTexture
{
public:
	virtual bool loadFromFile(CString filename, int xs, int ys)
		//{return CTexture::loadFromFile(filename, xs, ys);}
		{return true;}
};

#endif
