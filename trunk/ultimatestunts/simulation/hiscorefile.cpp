/***************************************************************************
                          hiscorefile.cpp  -  Loading and saving hiscores
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

#include <cstdio>

#include "datafile.h"
#include "hiscorefile.h"

CHiscoreFile::CHiscoreFile(const CString &trackfile)
{
	//change extension
	int extpos = trackfile.inStr(".track");
	m_HiscoreFile = trackfile.mid(0, extpos) + ".high";

	printf("Using hiscore file %s\n", m_HiscoreFile.c_str());

	load();
}

CHiscoreFile::~CHiscoreFile()
{
}

void CHiscoreFile::addEntries(const vector<SHiscoreEntry> &entries)
{
	for(unsigned int newe=0; newe < entries.size(); newe++)
	{
		float time = entries[newe].time;
		if(time < 0) continue; //crashed, stopped etc.

		bool inserted = false;

		for(unsigned int i=0; i < m_Entries.size(); i++)
			if(m_Entries[i].time > time)
			{
				m_Entries.insert(m_Entries.begin()+i, entries[newe]);
				inserted = true;
				break;
			}

		if(!inserted)
			m_Entries.push_back(entries[newe]);
	}

	if(m_Entries.size() > 20) m_Entries.resize(20);

	save();
}

bool CHiscoreFile::load()
{
	CDataFile f(m_HiscoreFile, false);

	m_Entries.clear();

	while(true)
	{
		CString name = f.readl();
		CString carname = f.readl();
		CString time = f.readl();

		if(name == "\n" || time == "\n") break;

		SHiscoreEntry e;
		e.name = name;
		e.carname = carname;
		e.time = time.toFloat();
		e.isNew = false;

		m_Entries.push_back(e);
	}

	return true;
}

bool CHiscoreFile::save()
{
	CDataFile f(m_HiscoreFile, true);

	for(unsigned int i=0; i < m_Entries.size(); i++)
	{
		SHiscoreEntry e = m_Entries[i];
		
		f.writel(e.name);
		f.writel(e.carname);
		f.writel(e.time);
	}

	return true;
}

