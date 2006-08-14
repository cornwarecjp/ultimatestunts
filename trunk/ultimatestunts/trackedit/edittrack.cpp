/***************************************************************************
                          edittrack.cpp  -  An editable track
                             -------------------
    begin                : ma mei 23 2005
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

#include "edittrack.h"
#include "datafile.h"

#include "trkfile.h"
#include "lconfig.h"

CEditTrack::CEditTrack(CDataManager *manager) : CTrack(manager)
{
}

CEditTrack::~CEditTrack()
{
}

bool CEditTrack::save(const CString &filename) const
{
	//Open the track file
	CDataFile tfile;
	if(!tfile.open(filename, true)) return false; //error on opening the file

	tfile.writel("TRACKFILE");
	tfile.writel(m_L);
	tfile.writel(m_W);
	tfile.writel(m_H);

	tfile.writel("");
	tfile.writel("#properties:");
	tfile.writel("#  length");
	tfile.writel("#  width");
	tfile.writel("#  height");
	tfile.writel("");
	tfile.writel("#This track file is created by the Ultimate Stunts track editor");
	tfile.writel("");

	//Write the materials section
	tfile.writel("BEGIN");
	vector<CDataObject *> materials = m_DataManager->getObjectArray(CDataObject::eMaterial);
	for(unsigned int i=0; i < materials.size(); i++)
	{
		const CDataObject *o = materials[i];
		CString filename = o->getFilename();
		const CParamList &params = o->getParamList();

		CString line = filename;
		for(unsigned int j=0; j < params.size(); j++)
		{
			line += " " + params[j].name + "=" + params[j].value;
		}

		tfile.writel(line);
	}
	tfile.writel("END");
	tfile.writel("");

	//Write the environment section
	tfile.writel("BEGIN");
	tfile.writel(m_BackgroundFilename);
	tfile.writel(m_EnvMapFilename);
	tfile.writel(m_LightDirection);
	tfile.writel(m_LightColor);
	tfile.writel(m_AmbientColor);
	tfile.writel("END");
	tfile.writel("");

	//Write the tiles section
	tfile.writel("BEGIN");
	vector<CDataObject *> tiles = m_DataManager->getObjectArray(CDataObject::eTileModel);
	for(unsigned int i=0; i < tiles.size(); i++)
	{
		const CDataObject *o = tiles[i];
		CString filename = o->getFilename();
		const CParamList &params = o->getParamList();
		CString subset = params.getValue("subset", "");
		CString flags = params.getValue("flags", "");

		CString line = filename;
		if(flags != "")
			line += ":" + flags;

		if(subset != "")
			line += " " + subset;

		tfile.writel(line);
	}
	tfile.writel("END");
	tfile.writel("");

	tfile.writel("#tile_id/rotation/heigth");
	tfile.writel("");


	//Write the track sections
	for(int y=0; y<m_H; y++)
	{
		tfile.writel("BEGIN");

		for(int z=0; z<m_W; z++)
		{
			CString line;

			for(int x=0; x<m_L; x++)
			{
				int n = y + m_H * (z+m_W*x);
				const STile &t = m_Track[n];
				line += CString("\t") + t.m_Model + "/" + t.m_R + "/" + t.m_Z;
			}

			tfile.writel(line);
       }

		tfile.writel("END");
		tfile.writel("");
	}


	//Write dummy route section
	tfile.writel("BEGIN");
	tfile.writel("END");
	tfile.writel("");

	return true;
}

bool CEditTrack::import(const CString &filename)
{
	//Load all the data from the file
	CTRKFile trk;
	if(!trk.load(filename))
		return false;

	CLConfig conf(CDataFile("misc/trackedit.conf").useExtern());
	CString templatefile = conf.getValue("trkimport", "templatefile");

	if(templatefile == "")
	{
		printf("misc/trackedit.conf does not contain a trkimport/templatefile value\n");
		return false;
	}
	if(!dataFileExists(templatefile, true))
	{
		printf("Couldn\'t find template file \"%s\"\n", templatefile.c_str());
		return false;
	}

	//Reset the state of the track
	m_DataManager->unloadAll(CDataObject::eTileModel);
	m_DataManager->unloadAll(CDataObject::eMaterial);

	//Load the template track
	load(templatefile, CParamList());

	for(unsigned int y=0; y<30; y++)
	for(unsigned int x=0; x<30; x++)
	{
		CTRKFile::STile tile = trk.m_Track[x][y];
		unsigned int offset = m_H * (x + m_W*y);

		CString confitem;

		//Terrain handling
		switch(tile.terrain)
		{
		//Terrain where things like bridges can be placed on top:
		case 0x0:
		case 0x6:
			//flat land: do nothing
			break;


		//Terrain where ony some items can be placed:
		case 0x7:
			placeItem(offset, conf.getValue("trkimport", "terrain_07"));
			if(tile.item != 0)
			{
				confitem = CString("terrain_07_") + hexStr(tile.item);
				placeItem(offset, conf.getValue("trkimport", confitem));
			}
			break;
		case 0x8:
			placeItem(offset, conf.getValue("trkimport", "terrain_08"));
			if(tile.item != 0)
			{
				confitem = CString("terrain_08_") + hexStr(tile.item);
				placeItem(offset, conf.getValue("trkimport", confitem));
			}
			break;
		case 0x9:
			placeItem(offset, conf.getValue("trkimport", "terrain_09"));
			if(tile.item != 0)
			{
				confitem = CString("terrain_09_") + hexStr(tile.item);
				placeItem(offset, conf.getValue("trkimport", confitem));
			}
			break;
		case 0xa:
			placeItem(offset, conf.getValue("trkimport", "terrain_0a"));
			if(tile.item != 0)
			{
				confitem = CString("terrain_0a_") + hexStr(tile.item);
				placeItem(offset, conf.getValue("trkimport", confitem));
			}
			break;


		default:
			confitem = CString("terrain_") + hexStr(tile.terrain);
			placeItem(offset, conf.getValue("trkimport", confitem));
			break;
		}

		//No simple addition of items here:
		if(tile.terrain >= 0x7)
			continue;

		//Tile handling
		confitem = CString("tile_") + hexStr(tile.item);
		confitem = conf.getValue("trkimport", confitem);
		if(confitem == "")
		{
			if(tile.item != 0x0 && tile.item != 0xfd && tile.item != 0xfe && tile.item != 0xff)
				printf("Stunts track import warning: item type %x not recognised\n", tile.item);
		}
		else
		{
			placeItem(offset, confitem);
		}

		//Increase height for high terrain
		if(tile.terrain == 0x6)
			for(int i=0; i<m_H; i++)
				m_Track[offset+i].m_Z++;
	}

	return true;
}

void CEditTrack::placeItem(unsigned int offset, const CString &item)
{
	CString s = item;
	while(true)
	{
		int sep = s.inStr(',');
		if(sep < 0) break;
		int x = s.mid(0, sep).toInt();
		s = s.mid(sep+1);

		sep = s.inStr(',');
		if(sep < 0) break;
		int y = s.mid(0, sep).toInt();
		s = s.mid(sep+1);

		sep = s.inStr(':');
		if(sep < 0) break;
		int z = s.mid(0, sep).toInt();
		s = s.mid(sep+1);

		sep = s.inStr('/');
		if(sep < 0) break;
		int model = s.mid(0, sep).toInt();
		s = s.mid(sep+1);

		sep = s.inStr('/');
		if(sep < 0) break;
		int rot = s.mid(0, sep).toInt();
		s = s.mid(sep+1);

		sep = s.inStr(';');
		if(sep < 0) break;
		int height = s.mid(0, sep).toInt();
		s = s.mid(sep+1);

		//Place the item
		unsigned int index = offset + z + m_H * (y + m_W*x); //I don't know why x and y are not the other way around
		m_Track[index].m_Model = model;
		m_Track[index].m_R = rot;
		m_Track[index].m_Z = height;
	}
}

CString CEditTrack::hexStr(unsigned char x)
{
	CString ret = "00";

	unsigned char a = x >> 4;
	unsigned char b = x & 15;

	if(a < 10)
		{ret[0] = '0' + a;}
	else
		{ret[0] = 'a' + a - 10;}

	if(b < 10)
		{ret[1] = '0' + b;}
	else
		{ret[1] = 'a' + b - 10;}

	//printf("  x = %d (%x); a = %d (%x); b = %d (%x); ret = %s\n", x, x, a, a, b, b, ret.c_str());

	return ret;
}
