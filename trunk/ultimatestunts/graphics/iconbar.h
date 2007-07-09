/***************************************************************************
                          iconbar.h  -  A toolbar with icons
                             -------------------
    begin                : za jun 9 2007
    copyright            : (C) 2007 by CJP
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
#ifndef ICONBAR_H
#define ICONBAR_H

#include <vector>
namespace std {}
using namespace std;

#include "iconlist.h"
#include "widget.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CIconBar : public CWidget
{
public:
	CIconBar();
	virtual ~CIconBar();

	virtual int onMouseMove(int x, int y, unsigned int buttons);
	virtual int onMouseClick(int x, int y, unsigned int buttons);
	virtual int onRedraw();

	struct SIcon
	{
		CIconList::eIconID ID;
		float xpos;
		CString description;
	};

	vector<SIcon> m_Icons;
	void addIcon(CIconList::eIconID ID, float xpos, const CString &descr)
	{
		SIcon icon;
		icon.ID = ID;
		icon.xpos = xpos;
		icon.description = descr;
		m_Icons.push_back(icon);
	}

	CIconList::eIconID m_Selected;
	CString m_Description;
};

#endif
