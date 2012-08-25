/***************************************************************************
                          label.h  -  A simple text label widget
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
#ifndef LABEL_H
#define LABEL_H

#include "cstring.h"
#include "widget.h"

/**
	@author CJP <cornware-cjp@users.sourceforge.net>
*/
class CLabel : public CWidget
{
public:
	CLabel();
	virtual ~CLabel();

	virtual int onRedraw();

	CString m_Text;
};

#endif
