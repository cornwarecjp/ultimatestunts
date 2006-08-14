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
#include "world.h"

CTrack::CTrack(CDataManager *manager) : CDataObject(manager, CDataObject::eTrack)
{
	m_L = m_W = m_H = 0;
}

CTrack::~CTrack()
{
	unload();
}

bool CTrack::load(const CString &filename, const CParamList &list)
{
	CDataObject::load(filename, list);

	//Open the track file
	CDataFile tfile(m_Filename);
	//printf("   The world is being loaded from %s\n\n", tfile.getName().c_str());

	CString line = tfile.readl();
	if(line != "TRACKFILE")
	{
		printf("   Track file \"%s\" does not contain a correct header\n", filename.c_str());
		return false;
	}

	m_L = tfile.readl().toInt();
	m_W = tfile.readl().toInt();
	m_H = tfile.readl().toInt();

	//First: load materials/textures
	//Find "BEGIN"
	if(!findBeginTag(tfile))
	{
		printf("Error: could not find the textures section in the track file\n");
		return false;
	}
	while(true)
	{
		line = tfile.readl();
		if(line == "END") break;

		CParamList plist;
		
		int pos = line.inStr("scale=");
		if(pos  > 0) //scale is specified
		{
			if((unsigned int)pos < line.length()-6)
			{
				SParameter p;
				p.name = "scale";
				p.value = line.mid(pos+6, line.length()-pos-6);
				plist.push_back(p);
			}
			//TODO: check for different y-direction mul
		}

		pos = line.inStr("mu=");
		if(pos  > 0) //mu is specified
		{
			if((unsigned int)pos < line.length()-3)
			{
				SParameter p;
				p.name = "mu";
				p.value = line.mid(pos+3, line.length()-pos-3);
				plist.push_back(p);
			}
		}

		pos = line.inStr(' ');
		if(pos  > 0) //there is a space
			line = line.mid(0, pos);

		//TODO: check if the ID is correct (starting from 0)
		m_DataManager->loadObject(line, plist, CDataObject::eMaterial);
	}
	printf("\n   Loaded %d materials\n\n", m_DataManager->getNumObjects(CDataObject::eMaterial));

	//Find "BEGIN"
	if(!findBeginTag(tfile))
	{
		printf("Error: could not find the environment section in the track file\n");
		return false;
	}
	m_BackgroundFilename = tfile.readl();
	m_EnvMapFilename = tfile.readl();
	m_LightDirection = tfile.readl().toVector();
	m_LightColor = tfile.readl().toVector();
	m_AmbientColor = tfile.readl().toVector();

	//Second: loading collision (and graphics) data
	//Find "BEGIN"
	if(!findBeginTag(tfile))
	{
		printf("Error: could not find the tiles section in the track file\n");
		return false;
	}
	while(true)
	{
		line = tfile.readl();
		if(line == "END") break;

		//printf("  line = \"%s\"\n", line.c_str());

		CString tile_flags, texture_indices;;
		int pos = line.inStr(' ');
		if(pos  > 0) //a space exists
		{
			if((unsigned int)pos < line.length()-1)
				{texture_indices = line.mid(pos+1, line.length()-pos-1);}
			line = line.mid(0, pos);
		}
		pos = line.inStr(':');
		if(pos > 0) //a : exists
		{
			if((unsigned int)pos < line.length()-1)
				{tile_flags = line.mid(pos+1, line.length()-pos-1);}
			line = line.mid(0, pos);
		}

		//printf("  flags = \"%s\"\n", tile_flags.c_str());

		CParamList plist;
		SParameter p;
		p.name = "subset";
		p.value = texture_indices;
		plist.push_back(p);
		p.name = "flags";
		p.value = tile_flags;
		plist.push_back(p);

		//TODO: check if the ID is correct (starting from 0)
		m_DataManager->loadObject(line, plist, CDataObject::eTileModel);
	}
	printf("\n   Loaded %d tile models\n\n", m_DataManager->getNumObjects(CDataObject::eTileModel));

	//Third: initialising track array
	m_Track.resize(m_L * m_W * m_H);
	for(int y=0; y<m_H; y++)
	{
		//Find "BEGIN"
		if(!findBeginTag(tfile))
		{
			printf("Error: could not find track layer %d section in the track file\n", y);
			return false;
		}

		for(int z=0; z<m_W; z++)
		{
			line = tfile.readl();

			for(int x=0; x<m_L; x++)
			{
				int n = y + m_H * (z+m_W*x);
				int tp = line.inStr('\t');
				line = line.mid(tp+1, line.length());

				STile &t = m_Track[n];
				t.m_Model = line.toInt();
				line = line.mid(line.inStr('/')+1, line.length());
				t.m_R = line.toInt();
				line = line.mid(line.inStr('/')+1, line.length());
				t.m_Z = line.toInt();

				//default values for rule data:
				t.m_Time = 0.0;
				t.m_RouteCounter = -1;
			}
       }

		if(tfile.readl()!="END") printf(
			"   Error while reading track data: END expected\n"
			"   after track layer %d\n"
			, y);

	}

	int s = m_Track.size();
	printf("   Loaded track aray: total %d tiles\n", s);

	//Fourth: loading the possible routes
	
	unsigned int counter = 0; //counts how many tiles we've had
	bool isAlternative = false; //Are we tracking an alternative (not fastest) route?
	float timeOffset = 0.0; //this is added to tile times

	//Find "BEGIN"
	if(!findBeginTag(tfile))
	{
		printf("Error: could not find the route section in the track file\n");
		return false;
	}

	while(true)
	{
		line = tfile.readl();
		if(line == "END") break;

		int pos = line.inStr(':');
		if(pos > 0)
		{
			CVector p = line.mid(0, pos).toVector();
			float time = line.mid(pos+1).toFloat();

			int x = (unsigned int)(p.x+0.1),
				y = (unsigned int)(p.z+0.1),
				z = (unsigned int)(p.y+0.1);
			STile &t = m_Track[y + m_H * (z+m_W*x)];

			//This doesn't work with the track editor:
			//bool isFinish = theWorld->getTileModel(t.m_Model)->m_isFinish;
			
			CDataObject *tmodel = m_DataManager->getObject(CDataObject::eTileModel, t.m_Model);
			bool isFinish = tmodel->getParamList().getValue("flags", "").inStr('f') >= 0;

			if(isFinish && counter > 0) //The finish counter and time
			{
				m_FinishRouteCounter = counter;
				m_FinishTime = timeOffset + time;
			}

			printf("%d,%d,%d: ", x,y,z);
			if(t.m_RouteCounter < 0) //Normal new piece of track
			{
				t.m_RouteCounter = counter;
				t.m_Time = timeOffset + time;
				printf("time = %.2f Counter = %d\n", t.m_Time, t.m_RouteCounter);
			}
			else //we've already been here: start or end of alternative route, or a loop finish
			{
				if(!isFinish)
				{
					if(isAlternative) //end of alternative route
					{
						printf("End of alternative route\n");
						isAlternative = false;
					}
					else //start of alternative route
					{
						counter = t.m_RouteCounter;
						timeOffset = t.m_Time;
						isAlternative = true;
						printf("New alternative counter=%d\n", counter);
					}
				}
			}

			counter++;
		}
	}

	printf("   Succesfully loaded the track\n");
	return true;
}

void CTrack::unload()
{
	m_L = m_W = m_H = 0;
	m_Track.clear();

	CDataObject::unload();
}

bool CTrack::findBeginTag(CDataFile &f)
{
	while(true)
	{
		CString line = f.readl();
		if(line == "BEGIN") return true;
		if(line == "\n") return false; //EOF
	}
}
