/***************************************************************************
                          background.h  -  A sky box
                             -------------------
    begin                : di feb 4 2003
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

#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <GL/gl.h>

#include "texture.h"
#include "timer.h"

/**
  *@author CJP
  */

class CBackground : public CTexture
{
public: 
	CBackground(CDataManager *manager);
	virtual ~CBackground();

	virtual bool load(const CString &filename, const CParamList &list);
	virtual void unload();

	CVector getFogColor() const
		{return m_FogColor;}

	CVector getClearColor() const;


	void draw() const;
protected:
	CTimer m_Timer;

	GLuint m_HorizonTex;

	bool m_ShowTextures, m_ShowGradient;

	CVector m_SkyColor;
	CVector m_HorizonSkyColor;
	CVector m_FogColor;
	CVector m_EnvironmentColor;

	void drawSkybox() const;
	void drawClouds() const;
	void drawHorizon() const;
};

#endif
