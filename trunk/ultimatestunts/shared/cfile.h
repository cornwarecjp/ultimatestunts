/***************************************************************************
                          cfile.h  -  description
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

#include <stdio.h> //voor FILE type
#include "cstring.h"

class CFile
{
	public:
		CFile(CString filename, bool write=false);
		~CFile();

		CString readl();
		void writel(CString l);

	protected:
		FILE * fp;
};

#endif
