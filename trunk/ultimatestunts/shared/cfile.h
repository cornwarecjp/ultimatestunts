/***************************************************************************
                          cfile.h  -  CJP's file class
                             -------------------
    begin                : Thu May 23 2002
    copyright            : (C) 2002 by CJP
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

#ifndef CFILE_H
#define CFILE_H

#include <cstdio> //voor FILE type

#include <vector>
namespace std {}
using namespace std;

#include "cstring.h"
#include "binbuffer.h"

class CFile
{
	public:
		CFile(CString filename, bool write=false);
		CFile();
		virtual ~CFile();

		virtual bool open(CString filename, bool write=false);
		virtual void close();
		virtual void reopen();

		virtual CString readl();
		virtual void writel(CString l);

		virtual CBinBuffer readBytes(unsigned int maxlen);
		virtual void writeBytes(const CBinBuffer &b);

		virtual CString getName()
			{return m_Filename;}

	protected:
		FILE * fp;
		CString m_Filename;
		bool m_Write;
};

bool fileExists(const CString &filename);
bool dirExists(const CString &dirname);
vector<CString> getDirContents(const CString &dir, const CString &ext = "");
bool makeDir(const CString &dirname);
bool makeFile(const CString &filename); //also makes top directories
bool copyFile(const CString &src, const CString &dest);

#endif
