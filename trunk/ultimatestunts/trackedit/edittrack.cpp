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

bool CEditTrack::tileIsUsed(int ID) const
{
	for(int i=0; i < m_L*m_W*m_H; i++)
	{
		const STile &t = m_Track[i];
		if(t.m_Model == ID) return true;
	}

	return false;
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
	tfile.writel(CString("sky = ") + m_SkyFilename);
	tfile.writel(CString("horizon = ") + m_HorizonFilename);
	tfile.writel(CString("envmap = ") + m_EnvMapFilename);
	tfile.writel(CString("lightdir = ") + m_LightDirection);
	tfile.writel(CString("lightcol = ") + m_LightColor);
	tfile.writel(CString("ambientcol = ") + m_AmbientColor);
	tfile.writel(CString("skycol = ") + m_SkyColor);
	tfile.writel(CString("horizonskycol = ") + m_HorizonSkyColor);
	tfile.writel(CString("fogcol = ") + m_FogColor);
	tfile.writel(CString("envcol = ") + m_EnvironmentColor);
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


	//Write route section
	tfile.writel("BEGIN");

	for(unsigned int r=0; r < m_Routes.size(); r++)
	{
		tfile.writel("");
		tfile.writel(CString("#Route ") + r);
		for(unsigned int i=0; i < m_Routes[r].size(); i++)
		{
			CCheckpoint cp = m_Routes[r][i];
			bool isLast = (i == m_Routes[r].size()-1);

			for(int h=0; h < m_H; h++)
			{
				const STile &t = m_Track[h + m_H * (cp.z+m_W*cp.x)];
				if(t.m_Z == cp.y)
				{
					cp.y = h;
					break;
				}
			}

			CString line = CString(cp.x) + "," + cp.z + "," + cp.y + ":";

			cp = m_Routes[r][i];
			//printf("%d, %d, %d -> %s\n", cp.x, cp.y, cp.z, line.c_str());

			if(isLast)
				line += " end";

			tfile.writel(line);
		}
	}

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

	//Skybox handling
	switch(trk.m_Skybox)
	{
	case 0: //desert
		m_HorizonFilename = conf.getValue("trkimport", "background_desert");
		break;
	case 1: //tropical
		m_HorizonFilename = conf.getValue("trkimport", "background_tropical");
		break;
	case 2: //alpine
		m_HorizonFilename = conf.getValue("trkimport", "background_alpine");
		break;
	case 3: //city
		m_HorizonFilename = conf.getValue("trkimport", "background_city");
		break;
	case 4: //country
		m_HorizonFilename = conf.getValue("trkimport", "background_country");
		break;
	}

	//Load tile data
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

	//delete all existing routes
	m_Routes.clear();

	//Follow all the routes in the trk file
	//First find the start tile and its orientation
	for(unsigned int x=0; x<30; x++)
	for(unsigned int z=0; z<30; z++)
	for(unsigned int y=0; y<(unsigned int)m_H; y++)
	{
		STile &t = m_Track[y + m_H * (z+m_W*x)];
		CDataObject *tmodel = m_DataManager->getObject(CDataObject::eTileModel, t.m_Model);
		if(tmodel->getParamList().getValue("flags", "").inStr('s') >= 0)
		{
			//Found it: now follow the routes
			CTrack::CCheckpoint start;
			start.x = x; start.y = t.m_Z; start.z = z;

			m_Routes.push_back(CTrack::CRoute());
			followTRKRoutes(trk, start, t.m_R);

			break;
		}
	}

	return true;
}

void CEditTrack::followTRKRoutes(const CTRKFile &file, CTrack::CCheckpoint start, int dir)
{
	if(m_Routes.size() >= 30*30) return;

	vector<CTrack::CCheckpoint> splitPoints;
	vector<int> splitDirs;

	while(true)
	{
		unsigned char item = file.m_Track[start.z][start.x].item;
		int altdir = -1; //meaning: no split or join, no skip

		switch(item)
		{
		case 0xfd:
			trackTRKCorners(
				item,
				file.m_Track[start.z-1][start.x-1].item,
				dir, start.y, altdir);
			break;
		case 0xfe:
			trackTRKCorners(
				item,
				file.m_Track[start.z-1][start.x].item,
				dir, start.y, altdir);
			break;
		case 0xff:
			trackTRKCorners(
				item,
				file.m_Track[start.z][start.x-1].item,
				dir, start.y, altdir);
			break;
		default:
			trackTRKCorners(
				file.m_Track[start.z][start.x].terrain,
				item,
				dir, start.y, altdir);
		};

		if(file.m_Track[start.z][start.x].terrain == 0x06)
			start.y++;

		//search for current position in existing routes
		bool found = false;
		for(unsigned int r=0; r < m_Routes.size(); r++)
		for(unsigned int i=0; i < m_Routes[r].size(); i++)
		{
			if(r == m_Routes.size()-1 && i == m_Routes[r].size()-1) continue; //don't check this

			if(m_Routes[r][i] == start)
			{
				found = true;
				break;
			}
		}

		if(found && item != 0x4a && item != 0x7d && item != 0x8a) //not a crosspoint
		{
			if(altdir == -2) //found a join
			{
				printf("Stopping route %d on a join\n", m_Routes.size()-1);
				m_Routes.back().push_back(start);
				break;
			}
			else
			{
				if(altdir != -3 && m_Routes.back().size() > 0)
				{
					//This is not a valid route, so undo it:
					printf("Collision with other route: undo routing\n");
					if(undoRoutingFromSplit(splitPoints))
					{
					printf("Continuing routing from last splits\n");
						start = splitPoints.back();
						dir = splitDirs.back();
						splitPoints.resize(splitPoints.size()-1);
						splitDirs.resize(splitDirs.size()-1);
						printf("  %d splits remaining on this route\n", splitPoints.size());
						continue;
					}
					else
					{
						break;
					}
				}
			}
		}

		//Add tile to current route
		printf("Following route %d: %d %d %d\n", m_Routes.size()-1, start.x, start.y, start.z);
		if(altdir != -3 && //-3 means skip
			( m_Routes.back().size()==0 || !(start == m_Routes.back().back()) ) //is different
			)
			m_Routes.back().push_back(start);

		if(altdir >= 0) //found a split
		{
			printf("Splitting the route\n");
			splitPoints.push_back(start);
			splitDirs.push_back(altdir);
		}

		//move forward
		switch(dir)
		{
		case 0:
			start.z--; break;
		case 1:
			start.x--; break;
		case 2:
			start.z++; break;
		case 3:
			start.x++; break;
		}

		if(start.x<0 || start.z<0 || start.x>=30 || start.z>=30)
		{
			printf("Over the edge: undo routing\n");
			//This is not a valid route, so undo it:
			if(undoRoutingFromSplit(splitPoints))
			{
				printf("Continuing routing from last splits\n");
				start = splitPoints.back();
				dir = splitDirs.back();
				splitPoints.resize(splitPoints.size()-1);
				splitDirs.resize(splitDirs.size()-1);
				continue;
			}
			else
			{
				break;
			}
		}

		//Check for finish tiles
		unsigned char newItem = file.m_Track[start.z][start.x].item;
		if(newItem == 0x01 ||
			newItem == 0x86 || newItem == 0x87 || newItem == 0x88 || newItem == 0x89 ||
			newItem == 0x93 || newItem == 0x94 || newItem == 0x95 || newItem == 0x96 ||
			newItem == 0xb3 || newItem == 0xb4 || newItem == 0xb5
			)
		{
			start.y = (file.m_Track[start.z][start.x].terrain==0x06);
			printf("Stopping route %d on finish %d %d %d\n", m_Routes.size()-1,
				start.x, start.y, start.z);
			m_Routes.back().push_back(start);
			break;
		}
	}

	//Process alternative routes
	for(unsigned int i=0; i < splitPoints.size(); i++)
	{
		m_Routes.push_back(CTrack::CRoute());
		followTRKRoutes(file, splitPoints[i], splitDirs[i]);
	}
}

bool CEditTrack::undoRoutingFromSplit
	(vector<CTrack::CCheckpoint> &splitpoints)
{
	if(m_Routes.size() < 2) return false;

	if(splitpoints.size() > 0)
	{
		//Delete until splitpoint
		while(true)
		{
			if(m_Routes.back().back() == splitpoints.back())
				break; //this is the split point

			//Otherwise this is not yet the splitpoint:
			m_Routes.back().resize(m_Routes.back().size()-1);
		}

		//And continue from splitpoint
		return true;
	}

	//Otherwise:

	//Delete completely
	m_Routes.resize(m_Routes.size()-1);

	//And don't continue
	return false;
}

void CEditTrack::trackTRKCorners(unsigned char terrain, unsigned char item, int &dir, int &height, int &altdir)
{
	//default:
	height = 0;
	altdir = -1;

	//Corners:
	switch(item)
	{
	//No road items:
	case 0x00:	case 0x97:
	case 0x98:	case 0x99:	case 0x9a:	case 0x9b:	case 0x9c:	case 0x9d:	case 0x9e:	case 0x9f:
	case 0xa0:	case 0xa1:	case 0xa2:	case 0xa3:	case 0xa4:	case 0xa5:	case 0xa6:	case 0xa7:
	case 0xa8:	case 0xa9:	case 0xaa:	case 0xab:	case 0xac:	case 0xad:	case 0xae:	case 0xaf:
	case 0xb0:	case 0xb1:	case 0xb2:
		altdir = -3;
		break;

	//Straight road items:
	case 0x04:	case 0x0e:	case 0x18:	case 0x22:	case 0x28:	case 0x2a:	case 0x2c:	case 0x2e:
	case 0x30:	case 0x31:	case 0x3a:	case 0x3b:	case 0x40:	case 0x42:	case 0x44:	case 0x46:
	case 0x47:	case 0x53:	case 0x55:	case 0x61:	case 0x62:	case 0x63:	case 0x67:	case 0x6d:
	case 0x6f:	case 0x71:	case 0x73:
		if(dir == 1 || dir == 3)
			altdir = -3;
		break;

	case 0x05:	case 0x0f:	case 0x19:	case 0x23:	case 0x29:	case 0x2b:	case 0x2d:	case 0x2f:
	case 0x32:	case 0x33:	case 0x38:	case 0x39:	case 0x41:	case 0x43:	case 0x45:	case 0x48:
	case 0x49:	case 0x54:	case 0x56:	case 0x5f:	case 0x60:	case 0x64:	case 0x68:	case 0x6e:
	case 0x70:	case 0x72:	case 0x74:
		if(dir == 0 || dir == 2)
			altdir = -3;
		break;

	//Sharp corners:
	case 0x06:
	case 0x10:
	case 0x1a:
		dir = (dir==0)? 3 : 2;
		break;
	case 0x07:
	case 0x11:
	case 0x1b:
		dir = (dir==0)? 1 : 2;
		break;
	case 0x08:
	case 0x12:
	case 0x1c:
		dir = (dir==2)? 3 : 0;
		break;
	case 0x09:
	case 0x13:
	case 0x1d:
		dir = (dir==2)? 1 : 0;
		break;

	//High-speed slaloms:
	case 0x3c:
		switch(terrain)
		{
		case 0xfd: if(dir==3){dir=0; altdir=-3;} break;
		case 0xfe: if(dir==0){dir=3;} break;
		case 0xff: if(dir==2){dir=1;} break;
		default:   if(dir==1){dir=2; altdir=-3;} break;
		};
		break;
	case 0x3d:
		switch(terrain)
		{
		case 0xfd: if(dir==2){dir=1; altdir=-3;} break;
		case 0xfe: if(dir==3){dir=0;} break;
		case 0xff: if(dir==1){dir=2;} break;
		default:   if(dir==0){dir=3; altdir=-3;} break;
		};
		break;
	case 0x3e:
		switch(terrain)
		{
		case 0xfd: if(dir==0){dir=1;} break;
		case 0xfe: if(dir==1){dir=0; altdir=-3;} break;
		case 0xff: if(dir==3){dir=2; altdir=-3;} break;
		default:   if(dir==2){dir=3;} break;
		};
		break;
	case 0x3f:
		switch(terrain)
		{
		case 0xfd: if(dir==1){dir=0;} break;
		case 0xfe: if(dir==2){dir=3; altdir=-3;} break;
		case 0xff: if(dir==0){dir=1; altdir=-3;} break;
		default:   if(dir==3){dir=2;} break;
		};
		break;

	//wide corners
	case 0x0a:
	case 0x14:
	case 0x1e:
	case 0x34:
	case 0x69:
		if(terrain <= 0x12)
		{
			dir = (dir==0)? 3 : 2;
			altdir = -3;
		}
		break;
	case 0x0b:
	case 0x15:
	case 0x1f:
	case 0x35:
	case 0x6a:
		if(terrain == 0xff)
		{
			dir = (dir==0)? 1 : 2;
			altdir = -3;
		}
		break;
	case 0x0c:
	case 0x16:
	case 0x20:
	case 0x36:
	case 0x6b:
		if(terrain == 0xfe)
		{
			dir = (dir==2)? 3 : 0;
			altdir = -3;
		}
		break;
	case 0x0d:
	case 0x17:
	case 0x21:
	case 0x37:
	case 0x6c:
		if(terrain == 0xfd)
		{
			dir = (dir==2)? 1 : 0;
			altdir = -3;
		}
		break;

	//Splits:
	case 0x4b:
	case 0x7e:
	case 0x8b:
		switch(dir)
		{
		case 0: altdir=1; break;
		case 2: altdir=-2; break;
		case 3: altdir=-2; dir=2; break;
		};
		break;
	case 0x4c:
	case 0x7f:
	case 0x8c:
		switch(dir)
		{
		case 1: altdir=2; break;
		case 3: altdir=-2; break;
		case 0: altdir=-2; dir=3; break;
		};
		break;
	case 0x4d:
	case 0x80:
	case 0x8d:
		switch(dir)
		{
		case 2: altdir=3; break;
		case 0: altdir=-2; break;
		case 1: altdir=-2; dir=0; break;
		};
		break;
	case 0x4e:
	case 0x81:
	case 0x8e:
		switch(dir)
		{
		case 3: altdir=0; break;
		case 1: altdir=-2; break;
		case 2: altdir=-2; dir=1; break;
		};
		break;
	case 0x4f:
	case 0x82:
	case 0x8f:
		switch(dir)
		{
		case 0: altdir=3; break;
		case 1: altdir=-2; dir=2; break;
		case 2: altdir=-2; break;
		};
		break;
	case 0x50:
	case 0x83:
	case 0x90:
		switch(dir)
		{
		case 1: altdir=0; break;
		case 2: altdir=-2; dir=3; break;
		case 3: altdir=-2; break;
		};
		break;
	case 0x51:
	case 0x84:
	case 0x91:
		switch(dir)
		{
		case 2: altdir=1; break;
		case 3: altdir=-2; dir=0; break;
		case 0: altdir=-2; break;
		};
		break;
	case 0x52:
	case 0x85:
	case 0x92:
		switch(dir)
		{
		case 3: altdir=2; break;
		case 0: altdir=-2; dir=1; break;
		case 1: altdir=-2; break;
		};
		break;

	//Wide splits:
	case 0x57:
		if(terrain == 0xff)
		switch(dir)
		{
		case 0: altdir=1; break;
		case 2: altdir=-2; break;
		case 3: altdir=-2; dir=2; break;
		};
		break;
	case 0x58:
		if(terrain <= 0x12)
		switch(dir)
		{
		case 1: altdir=2; break;
		case 3: altdir=-2; break;
		case 0: altdir=-2; dir=3; break;
		};
		break;
	case 0x59:
		if(terrain == 0xfe)
		switch(dir)
		{
		case 2: altdir=3; break;
		case 0: altdir=-2; break;
		case 1: altdir=-2; dir=0; break;
		};
		break;
	case 0x5a:
		if(terrain == 0xfd)
		switch(dir)
		{
		case 3: altdir=0; break;
		case 1: altdir=-2; break;
		case 2: altdir=-2; dir=1; break;
		};
		break;
	case 0x5b:
		if(terrain <= 0x12)
		switch(dir)
		{
		case 0: altdir=3; break;
		case 1: altdir=-2; dir=2; break;
		case 2: altdir=-2; break;
		};
		break;
	case 0x5c:
		if(terrain == 0xfe)
		switch(dir)
		{
		case 1: altdir=0; break;
		case 2: altdir=-2; dir=3; break;
		case 3: altdir=-2; break;
		};
		break;
	case 0x5d:
		if(terrain == 0xfd)
		switch(dir)
		{
		case 2: altdir=1; break;
		case 3: altdir=-2; dir=0; break;
		case 0: altdir=-2; break;
		};
		break;
	case 0x5e:
		if(terrain == 0xff)
		switch(dir)
		{
		case 3: altdir=2; break;
		case 0: altdir=-2; dir=1; break;
		case 1: altdir=-2; break;
		};
		break;
	};

	//Bridges:
	switch(item)
	{
	case 0x22:
	case 0x23:
	case 0x63:
	case 0x64:
	case 0x67:
	case 0x68:
	case 0x69:
	case 0x6a:
	case 0x6b:
	case 0x6c:
		height = 1;
		break;

	//Viaducts
	case 0x65:
		height = (dir==0 || dir==2);
		break;
	case 0x66:
		height = (dir==1 || dir==3);
		break;

	//Start on hill
	case 0x24:
	case 0x38:
	case 0x5f:

	case 0x25:
	case 0x39:
	case 0x60:

	case 0x26:
	case 0x3a:
	case 0x61:

	case 0x27:
	case 0x3b:
	case 0x62:
		height = (terrain == 0x07 || terrain == 0x08 || terrain == 0x09 || terrain == 0x0a);
		break;
	}
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
