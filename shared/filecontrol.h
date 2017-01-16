/***************************************************************************
                          filecontrol.h  -  Controlling the behaviour of datafiles
                             -------------------
    begin                : do mei 1 2003
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

#include "cstring.h"

#ifndef FILECONTROL_H
#define FILECONTROL_H

class CFileControl
{
public:
	CFileControl(bool takeover = true);
	virtual ~CFileControl();

	void setDataDir(const CString &dir);
	void setSaveDir(const CString &dir);

	virtual CString getLongFilename(const CString &shortName); //for reading only!

	CString filecontroldatadir;
	CString filecontrolsavedir;
};

extern CFileControl *theFileControl;

#endif
