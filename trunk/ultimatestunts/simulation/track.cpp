/***************************************************************************
                          track.cpp  -  A racing track
                             -------------------
    begin                : di dec 7 2004
    copyright            : (C) 2004 by CJP
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
#include "track.h"
#include "datafile.h"

CTrack::CTrack(CDataManager *manager) : CDataObject(manager, CDataObject::eTrack)
{
	m_L = m_W = m_H = 0;
}

CTrack::~CTrack()
{
}

bool CTrack::load(const CString &idstring)
{
	CDataObject::load(idstring);

	//Open the track file
	CDataFile tfile(m_Filename);
	printf("   The world is being loaded from %s\n\n", tfile.getName().c_str());

	CString line = tfile.readl();
	if(line != "TRACKFILE")
	{
		printf("   Track file does not contain a correct header\n");
		return false;
	}

	m_L = tfile.readl().toInt();
	m_W = tfile.readl().toInt();
	m_H = tfile.readl().toInt();

	//First: load materials/textures
	while(tfile.readl() != "BEGIN"); //begin of textures section
	while(true)
	{
		line = tfile.readl();
		if(line == "END") break;

		//TODO: check if the ID is correct (starting from 0)
		m_DataManager->getObjectID(line, CDataObject::eMaterial);
	}
	printf("\n   Loaded %d materials\n\n", m_DataManager->getNumObjects(CDataObject::eMaterial));

	while(tfile.readl() != "BEGIN"); //Begin of background section
	m_BackgroundFilename = tfile.readl();
	m_EnvMapFilename = tfile.readl();

	//Second: loading collision (and graphics) data
	while(tfile.readl() != "BEGIN"); //begin of tiles section
	while(true)
	{
		line = tfile.readl();
		if(line == "END") break;

		//TODO: check if the ID is correct (starting from 0)
		m_DataManager->getObjectID(line, CDataObject::eTileModel);
	}
	printf("\n   Loaded %d tile models\n\n", m_DataManager->getNumObjects(CDataObject::eTileModel));

	//Third: initialising track array
	m_Track.resize(m_L * m_W * m_H);
	for(int y=0; y<m_H; y++)
	{
		while(tfile.readl() != "BEGIN"); //begin of track layer section

		for(int z=0; z<m_W; z++)
		{
			line = tfile.readl();

			for(int x=0; x<m_L; x++)
			{
				int n = y + m_H * (z+m_W*x);
				int tp = line.inStr('\t');
				line = line.mid(tp+1, line.length());

				STile t;
				t.m_Model = line.toInt();
				line = line.mid(line.inStr('/')+1, line.length());
				t.m_R = line.toInt();
				line = line.mid(line.inStr('/')+1, line.length());
				t.m_Z = line.toInt();
				m_Track[n] = t;
			}
       }

		if(tfile.readl()!="END") printf(
			"   Error while reading track data: END expected\n"
			"   after track layer %d\n"
			, y);

	}

	int s = m_Track.size();
	printf("   Loaded the track: total %d tiles\n", s);

	return true;
}

void CTrack::unload()
{
	m_L = m_W = m_H = 0;
	m_Track.clear();

	CDataObject::unload();
}
