/***************************************************************************
                          editrenderer.h  -  The renderer of stunts3dedit
                             -------------------
    begin                : wo apr 9 2003
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

#ifndef EDITRENDERER_H
#define EDITRENDERER_H

#include "renderer.h"
#include "graphobj.h"
#include "texture.h"

/**
  *@author CJP
  */

class CEditRenderer : public CRenderer  {
public: 
	CEditRenderer();
	virtual ~CEditRenderer();

	void setGraphobj(const CGraphObj *obj)
		{m_GraphObj = obj;}

	virtual void update();

protected:
	const CGraphObj *m_GraphObj;

	CTexture *m_EnvMap;
};

#endif
