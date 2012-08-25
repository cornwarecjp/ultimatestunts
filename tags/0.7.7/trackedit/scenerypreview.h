/***************************************************************************
                          scenerypreview.h  -  A widget for previewing track scenery
                             -------------------
    begin                : wo dec 22 2010
    copyright            : (C) 2010 by CJP
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
#ifndef SCENERYPREVIEW_H
#define SCENERYPREVIEW_H

#include "widget.h"
#include "texture.h"
#include "track.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CSceneryPreview : public CWidget
{
public:
	CSceneryPreview();
	virtual ~CSceneryPreview();
	
	virtual int onRedraw();

	CTrack::SEnv m_Environment;

protected:
	void setColor(const CVector &color) const;

	CTexture m_SkyTexture;
	CTexture m_BackgroundTexture;

	CTexture m_SunTexture;
};

#endif
