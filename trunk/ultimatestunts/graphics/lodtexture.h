/***************************************************************************
                          lodtexture.h  -  Texture with multiple Levels Of Detail
                             -------------------
    begin                : do okt 28 2004
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

#ifndef LODTEXTURE_H
#define LODTEXTURE_H

#include "texture.h"

/**
  *@author CJP
  */

class CLODTexture : public CTexture  {
public: 
	CLODTexture(CDataManager *manager);
	virtual ~CLODTexture();

	virtual void unload();

	int getSizeX(int i) const;
	int getSizeY(int i) const;
	void draw(int lod) const;

protected:
	unsigned int m_Texture2;
	unsigned int m_Texture3;
	unsigned int m_Texture4;

	int sizex2,sizey2,
			sizex3,sizey3,
			sizex4,sizey4;

	virtual RGBImageRec *loadFromImage(RGBImageRec *in_image, int xs, int ys);
};

#endif
