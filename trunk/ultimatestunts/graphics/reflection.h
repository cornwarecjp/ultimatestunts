/***************************************************************************
                          reflection.h  -  A dynamic reflection map
                             -------------------
    begin                : ma sep 20 2004
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

#ifndef REFLECTION_H
#define REFLECTION_H

#include "renderer.h"
#include "graphicsettings.h"

/**
  *@author CJP
  */

class CReflection  {
public: 
	CReflection(bool smooth, unsigned int size);
	~CReflection();

	void update(CRenderer *renderer, CCamera *cam, int side = -1);

	void enable(const SGraphicSettings *settings);
	void disable();

protected:
	const SGraphicSettings *m_CurrentSettings;

	unsigned int m_Texture; //spheremap texture
	unsigned int m_ReflectionTexture[6]; //side textures
	float m_CentralPixel[4]; //the front color = clear color for the spheremap

	float oldambient[4]; //temporary place to put the ambient color


	void initialiseReflections();

	unsigned int m_Size;
	bool m_TexSmooth;
};

#endif
