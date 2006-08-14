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

//ONLY USE DERIVED CLASSES (CStaticReflection and CDynamicReflection)
class CReflection  {
public: 
	CReflection();
	~CReflection();

	void enable(const SGraphicSettings *settings);
	void disable();

protected:
	unsigned int m_Texture; //spheremap texture

	const SGraphicSettings *m_CurrentSettings;

	float oldambient[4]; //temporary place to put the ambient color
};

#endif
