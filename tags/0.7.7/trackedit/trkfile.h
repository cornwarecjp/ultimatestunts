/***************************************************************************
                          trkfile.h  -  An original Stunts track file
                             -------------------
    begin                : sa jul 29 2006
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

#ifndef TRKFILE_H
#define TRKFILE_H

#include <vector> //STL vector template
namespace std {}
using namespace std;

#include "cstring.h"

/**
  *@author CJP
  */

class CTRKFile {
public: 
	CTRKFile();
	~CTRKFile();

	bool load(const CString &filename);

	struct STile
	{
		unsigned char terrain;
		unsigned char item;
	};

	vector<vector<STile> > m_Track;

	unsigned char m_Skybox;
};

#endif
