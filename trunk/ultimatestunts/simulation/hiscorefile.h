/***************************************************************************
                          hiscorefile.h  -  Loading and saving hiscores
                             -------------------
    begin                : wo aug 31 2005
    copyright            : (C) 2005 by CJP
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

#ifndef HISCOREFILE_H
#define HISCOREFILE_H


/**
  *@author CJP
  */

#include <vector>
namespace std {}
using namespace std;

#include "cstring.h"

struct SHiscoreEntry
{
	CString name;
	CString carname;
	float time;
	bool isNew; //is it from this race or not? (not saved in file of course)
};

class CHiscoreFile {
public: 
	CHiscoreFile(const CString &trackfile);
	~CHiscoreFile();

	void addEntries(const vector<SHiscoreEntry> &entries);

	vector<SHiscoreEntry> getEntries()
		{return m_Entries;}

protected:
	CString m_HiscoreFile;
	vector<SHiscoreEntry> m_Entries;

	bool load();
	bool save();
};

#endif
