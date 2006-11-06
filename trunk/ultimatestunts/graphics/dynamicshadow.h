/***************************************************************************
                          dynamicshadow.h  -  An updatable shadow projection texture
                             -------------------
    begin                : do jan 26 2006
    copyright            : (C) 2006 by CJP
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

#ifndef DYNAMICSHADOW_H
#define DYNAMICSHADOW_H

/**
  *@author CJP
  */

#include "matrix.h"

class CGraphicWorld;

class CDynamicShadow {
public: 
	CDynamicShadow(unsigned int size, CGraphicWorld *graphworld, unsigned int objID);
	~CDynamicShadow();

	void enable();
	void disable();

	void setLightSource(const CVector &v, const CVector color)
	{
		m_LightOrientation.targetZ(-v);
		m_ShadowColor = color;
	}
	const CMatrix &getLightOrientation()
		{return m_LightOrientation;}

	float getPhysicalSize()
		{return m_PhysicalSize;}

	void update(const SGraphicSettings *settings, float t);
protected:
	void initialiseShadow();

	float oldambient[4]; //temporary place to put the ambient color

	CMatrix m_LightOrientation;
	CVector m_ShadowColor;

	unsigned int m_Size;
	unsigned int m_Texture;
	float m_PhysicalSize;

	CGraphicWorld *m_GraphicWorld;
	unsigned int m_MovObjID;
};

#endif
