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

#include <dirent.h>
#include <cstdio>

#include "filecontrol.h"
#include "datafile.h"

//Shared data part:

CString filecontroldatadir;

//CFileControl part

CFileControl::CFileControl()
{}

CFileControl::~CFileControl()
{}

void CFileControl::setDataDir(const CString &dir)
{
	filecontroldatadir = dir;
}

//CDataFile part

CDataFile::CDataFile(CString filename, bool write)
{
	open(filename, write);
}

CDataFile::~CDataFile()
{
	//I gues it's already being closed by CFile
}

bool CDataFile::open(CString filename, bool write)
{
	//Do something special
	return CFile::open(filecontroldatadir + filename, write);
}

CString CDataFile::useExtern()
{
	CFile::close();
	return m_Filename;
}

CString getShortName(const CString &longname)
{
	int pos = longname.inStr(filecontroldatadir);
	if(pos < 0) return "";

	return longname.mid(pos + filecontroldatadir.length());
}

vector<CString> getDirContents(const CString &dir, const CString &ext)
{
	vector<CString> ret;

	CString fullname = filecontroldatadir + dir;

	DIR *theDir = opendir(fullname.c_str());

	if(theDir == NULL) return ret;

	while(true)
	{
		struct dirent *entry = readdir(theDir);
		if(entry == NULL) break;

		CString entname = entry->d_name;
		//file extension check:
		if(ext == "" || entname.inStr(ext) == (int)(entname.length() - ext.length()))
			ret.push_back(entname);
	}

	closedir(theDir);
	return ret;
}
