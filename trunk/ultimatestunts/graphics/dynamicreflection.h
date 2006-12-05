/***************************************************************************
                          dynamicreflection.h  -  description
                             -------------------
    begin                : za mei 14 2005
    copyright            : (C) 2005 by CJP
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

#ifndef DYNAMICREFLECTION_H
#define DYNAMICREFLECTION_H

#include "reflection.h"

/**
  *@author CJP
  */

class CDynamicReflection : public CReflection  {
public: 
	CDynamicReflection(unsigned int size);
	~CDynamicReflection();

	void update(CRenderer *renderer, CCamera *cam, int side = -1);

protected:
	unsigned int m_ReflectionTexture[6]; //side textures
	float m_CentralPixel[4]; //the front color = clear color for the spheremap

	void initialiseReflections();

	bool m_WorkaroundTransferbug;
	unsigned int m_Size;
};

#endif
