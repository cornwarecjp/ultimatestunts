/***************************************************************************
                          datafile.cpp  -  datafile + filecontrol implementations
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

#include "datafile.h"

CDataFile::CDataFile(CString filename, bool write)
{
	open(filename, write);
}

CDataFile::~CDataFile()
{
	//I gues it's already being closed by CFile
}

bool CDataFile::open(CString filename, bool write=false)
{
	//Do something special
	CFile::open(filename, write);
}

