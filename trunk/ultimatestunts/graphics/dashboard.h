/***************************************************************************
                          dashboard.h  -  A dashboard for cars etc.
                             -------------------
    begin                : ma jan 30 2006
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

#ifndef DASHBOARD_H
#define DASHBOARD_H


/**
  *@author CJP
  */

#include "car.h"

#include "graphicworld.h"
#include "texture.h"
#include "vector.h"

class CDashboard {
public: 
	CDashboard(CGraphicWorld *manager, unsigned int movObjID);
	~CDashboard();

	enum eShowMode
	{eFull, eGauges, eTimer};
	void draw(unsigned int w, unsigned int h, eShowMode mode = eFull);

protected:
	CTexture *m_BackgroundTexture, *m_SteerTexture;

	CTexture *m_CrashBackgroundTexture;
	CTexture *m_CrashTexture;

	CTexture *m_VelGaugeTex, *m_RPMGaugeTex;

	SCarDashboardInfo m_Info;

	unsigned int m_MovObjID;

	CGraphicWorld *m_GraphicWorld;
};

#endif
