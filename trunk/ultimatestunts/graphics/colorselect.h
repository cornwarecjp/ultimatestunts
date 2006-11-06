/***************************************************************************
                          colorselect.h  -  A Color Selection tool
                             -------------------
    begin                : di sep 05 2006
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

#ifndef COLORSELECT_H
#define COLORSELECT_H

#include <vector>
namespace std {}
using namespace std;

#include "widget.h"
#include "cstring.h"
#include "vector.h"
/**
  *@author CJP
  */

class CColorSelect : public CWidget  {
public: 
	CColorSelect();
	virtual ~CColorSelect();

	virtual int onKeyPress(int key);
	virtual int onRedraw();
	virtual int onMouseMove(int x, int y, unsigned int buttons);
	virtual int onMouseClick(int x, int y, unsigned int buttons);

	void setTitle(const CString &title);

	CVector m_Color;

	bool m_Cancelled;

protected:
	CString m_Title; //the total text

	enum {eOK, eCancel, eNone} m_Selected;
};

#endif
