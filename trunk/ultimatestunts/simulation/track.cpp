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
				p.value = CString(p.value.toInt());
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
				p.value = CString(p.value.toFloat());
				plist.push_back(p);
			}
		}

		pos = line.inStr("roll=");
		if(pos  > 0) //roll is specified
		{
			if((unsigned int)pos < line.length()-5)
			{
				SParameter p;
				p.name = "roll";
				p.value = line.mid(pos+5, line.length()-pos-5);
				p.value = CString(p.value.toFloat());
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

	while(true)
	{
		CString line = tfile.readl();
		if(line == "END") break;

		int ispos = line.inStr('=');
		if(ispos <= 0) continue;

		CString name = line.mid(0, ispos), value = line.mid(ispos+1);
		name.Trim(); value.Trim();

		if(name == "sky")
			{m_SkyFilename = value;}
		else if(name == "horizon")
			{m_HorizonFilename = value;}
		else if(name == "envmap")
			{m_EnvMapFilename = value;}
		else if(name == "lightdir")
			{m_LightDirection = value.toVector();}
		else if(name == "lightcol")
			{m_LightColor = value.toVector();}
		else if(name == "ambientcol")
			{m_AmbientColor = value.toVector();}
		else if(name == "skycol")
			{m_SkyColor = value.toVector();}
	}

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
		if(pos > 0 && !(line.inStr('#') >=0 && pos > line.inStr('#')) )
		{
			CVector p = line.mid(0, pos).toVector();

			bool isEnd = line.mid(pos+1).toLower().inStr("end") >= 0;

			//New route tracker:
			CCheckpoint newPoint;
			newPoint.x = (int)(p.x+0.1);
			newPoint.y = (int)(p.z+0.1);
			newPoint.z = (int)(p.y+0.1);

			STile &t = m_Track[newPoint.y + m_H * (newPoint.z+m_W*newPoint.x)];
			CDataObject *tmodel = m_DataManager->getObject(CDataObject::eTileModel, t.m_Model);

			newPoint.y = t.m_Z; //height translation
			//printf("%.f, %.f, %.f -> %d %d %d\n", p.x, p.y, p.z, newPoint.x,newPoint.y,newPoint.z);

			//Check whether it already exists
			int exroute=-1, extile=-1;
			for(unsigned int r=0; r < m_Routes.size(); r++)
				for(int i=m_Routes[r].size()-1; i >= 0 ; i--)
					if(m_Routes[r][i] == newPoint)
					{
						exroute = (int)r; extile = i;
						break;
					}

			if(isEnd)
			{
				if(m_Routes.size() == 1) //End of basic route
				{
					bool isFinish = tmodel->getParamList().getValue("flags", "").inStr('f') >= 0;
					if(!isFinish)
					{
						printf("Error: end of first route is not a finish tile\n");
						return false;
					}

					//printf("End of first route\n");
				}
				else
				{
					if(exroute < 0)
					{
						printf("Error: end of new route is not on an existing route\n");
						return false;
					}

					//printf("End of new route\n");
					m_Routes.back().finishRoute = exroute;
					m_Routes.back().finishTile = extile;
				}

				//Normal adding to the latest route
				m_Routes.back().push_back(newPoint);

				//add new route
				CRoute newr;
				newr.finishRoute = newr.finishTile = newr.startRoute = newr.startTile = 0;
				m_Routes.push_back(newr);
			}
			else
			{
				if(m_Routes.size() == 0) //start of first route
				{
					bool isStart = tmodel->getParamList().getValue("flags", "").inStr('s') >= 0;
					if(!isStart)
					{
						printf("Error: start of first route is not a start tile\n");
						return false;
					}

					//Add first route:
					//printf("Start of first route\n");
					CRoute first;
					first.finishRoute = first.finishTile = first.startRoute = first.startTile = 0;
					m_Routes.push_back(first);
				}

				if(m_Routes.back().size() == 0 && m_Routes.size() > 1) //start of a new route
				{
					if(exroute < 0)
					{
						printf("Error: start of new route is not on an existing route\n");
						return false;
					}

					//printf("Start of new route\n");
					m_Routes.back().startRoute = exroute;
					m_Routes.back().startTile = extile;
				}

				//Normal adding to the latest route
				m_Routes.back().push_back(newPoint);
			}
		}
	}

	//Remove last empty track
	if(m_Routes.back().size() == 0)
	{
		m_Routes.resize(m_Routes.size()-1);
	}

	printf("   Succesfully loaded the track\n");
	return true;
}

void CTrack::unload()
{
	m_L = m_W = m_H = 0;
	m_Track.clear();
	m_Routes.clear();

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
