/***************************************************************************
                          texture.h  -  A texture class
                             -------------------
    begin                : Tue Jun 18 2002
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
#ifndef TEXTURE_H
#define TEXTURE_H
#include "image.h"
#include "vector.h"

#include "cstring.h"
#include "dataobject.h"

class CTexture : public CDataObject
{
public:
	CTexture(CDataManager *manager);
	virtual ~CTexture();

	virtual bool load(const CString &filename, const CParamList &list);
	virtual void unload();

	int getSizeX() const;
	int getSizeY() const;
	CVector getColor() const;

	void draw() const;
protected:
	RGBImageRec *loadImage(CString filename);
	virtual RGBImageRec *loadFromImage(RGBImageRec *in_image, int xs, int ys);
	void freeImage(RGBImageRec *image);

	RGBImageRec *scaleImage(RGBImageRec *in, int xs, int ys);

	bool m_TextureSmooth;

	unsigned int m_Texture;

	int m_Sizex, m_Sizey;

	CVector m_Color;
};

#endif
