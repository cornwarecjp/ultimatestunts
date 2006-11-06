/***************************************************************************
                          carviewer.h  -  Viewing cars
                             -------------------
    begin                : ma okt 30 2006
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

#ifndef CARVIEWER_H
#define CARVIEWER_H

#include "objectviewer.h"

/**
  *@author CJP
  */

class CCarViewer : public CObjectViewer {
public: 
	CCarViewer(const CWinSystem *winsys, CGraphicWorld *world);
	virtual ~CCarViewer();

	virtual void update();

	void loadCar(const CString &carfile, CVector color);

	virtual void reloadData(); //re-connects texture IDs to cars

protected:
	CCamera m_Camera;
};

#endif
