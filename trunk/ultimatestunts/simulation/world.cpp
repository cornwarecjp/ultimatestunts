/***************************************************************************
                          world.cpp  -  The world: tiles and cars and other objects
                             -------------------
    begin                : Wed Dec 4 2002
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
#include <stdio.h>

#include "world.h"
#include "car.h"
#include "datafile.h"

CWorld::CWorld()
{
	m_CollData = new CCollisionData(this);
}

CWorld::~CWorld(){
	unloadTrack();
	unloadMovObjs();

	delete m_CollData;
}

bool CWorld::loadTrack(CString filename)
{
	//Open the track file
	CDataFile tfile(filename);
	printf("   The world is being loaded from %s\n\n", tfile.getName().c_str());

	CString line = tfile.readl();
	if(line != "TRACKFILE")
		{printf("   Track file does not contain a correct header\n");}
		//TODO: throw something

	m_L = tfile.readl().toInt();
	m_W = tfile.readl().toInt();
	m_H = tfile.readl().toInt();

	//First: load materials/textures
	while(tfile.readl() != "BEGIN"); //begin of textures section
	while(true)
	{
		int mul = 1;
		line = tfile.readl();
		if(line == "END") break;
		int pos = line.inStr(' ');
		if(pos  > 0) //a space exists
		{
			if((unsigned int)pos < line.length()-1)
				{mul = line.mid(pos+1, line.length()-pos-1).toInt();}
			//TODO: check for different y-direction mul

			line = line.mid(0, pos);
		}

		CMaterial *m = new CMaterial;
		m->loadFromFile(line, mul);
		//TODO: set friction coefficients etc.
		m_TileMaterials.push_back(m);
	}
	printf("\n   Loaded %d materials\n\n", m_TileMaterials.size());

	while(tfile.readl() != "BEGIN"); //Begin of background section
	m_BackgroundFilename = tfile.readl();

	//Second: loading collision (and graphics) data
	while(tfile.readl() != "BEGIN"); //begin of tiles section
	while(true)
	{
		CString texture_indices;
		CString tile_flags;
		line = tfile.readl();
		if(line == "END") break;
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

		CTileShape *b = new CTileShape;

		CMaterial **subset = getMaterialSubset(texture_indices);
		//printf("Loading %s\n", line.c_str());
		b->loadFromFile(line, texture_indices, subset);
		delete [] subset;

		b->m_isStart = tile_flags.inStr('s') >= 0;
		b->m_isFinish = tile_flags.inStr('f') >= 0;
		b->m_Time = 1.0;

		m_TileShapes.push_back(b);
	}
	printf("\n   Loaded %d tile shapes\n\n", m_TileShapes.size());

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

				CTile t;
				t.m_Shape = line.toInt();
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

void CWorld::unloadTrack()
{
	printf("   Unloading track...\n");

	//The array containing the track
	m_Track.clear();


	if(m_TileShapes.size() > 0)
	{
		//The CShape objects themselves
		for(unsigned int i=0; i<m_TileShapes.size(); i++)
			delete m_TileShapes[i];

		//The array containing them
		m_TileShapes.clear();
	}

	if(m_TileMaterials.size() > 0)
	{
		//The materials/textures themselves
		for(unsigned int i=0; i<m_TileMaterials.size(); i++)
			delete m_TileMaterials[i];

		//The array containing them
		m_TileMaterials.clear();
	}
}

bool CWorld::loadMovObjs(CString filename)
{
	//Temporary solution: nr of objs put in string
	int num = filename.toInt();

	//First: load materials/textures
	//for example:
	/*
	CMaterial *m = createMaterial();
	m->loadFromFile("no_file.rgb", 256, 256); //Only useful for textures
	//TODO: set friction coefficients
	m_MovObjMaterials.push_back(m);
    */

	//Second: loading collision (and graphics) data
	//For example:
	{
		CBound *body = new CBound, *wheel = new CBound;
		body->loadFromFile("cars/body.gl", NULL);
		wheel->loadFromFile("cars/wheel.gl", NULL);
		m_MovObjBounds.push_back(body);
		m_MovObjBounds.push_back(wheel);
	}

	//Third: defining sound data
	{
		CString filename = "sounds/engine.wav";
		m_MovObjSounds.push_back(filename);
	}

	//Finally: Initialising car array
	for(int i=0; i<num; i++)
	{
		CMovingObject *m = new CCar; //making a general car object

		if(m->getType() != CMessageBuffer::car)
			printf("   Error: created car is not a car!\n");

		//setting the car settings
		m->m_Bodies[0].m_Body = 0; //The car's body
		m->m_Bodies[1].m_Body = 1; //The car's wheels
		m->m_Bodies[2].m_Body = 1;
		m->m_Bodies[3].m_Body = 1;
		m->m_Bodies[4].m_Body = 1;

		m->m_Sounds[0] = 0; //The engine sound

		m_MovObjs.push_back(m);

		int s = m_MovObjs.size();
		printf("   Added car: total %d moving objects\n", s);

	}

		return true;
}

void CWorld::unloadMovObjs()
{
	printf("   Unloading moving objects...\n");

	if(m_MovObjs.size() > 0)
	{
		//The objects themselves
		for(unsigned int i=0; i<m_MovObjs.size(); i++)
			delete m_MovObjs[i];

		//The array containing them
		m_MovObjs.clear();
	}

	if(m_MovObjBounds.size() > 0)
	{
		//The CBound objects themselves
		for(unsigned int i=0; i<m_MovObjBounds.size(); i++)
			delete m_MovObjBounds[i];

		//The array containing them
		m_MovObjBounds.clear();
	}

	if(m_MovObjMaterials.size() > 0)
	{
		//The materials/textures themselves
		for(unsigned int i=0; i<m_MovObjMaterials.size(); i++)
			delete m_MovObjMaterials[i];

		//The array containing them
		m_MovObjMaterials.clear();
	}

}

CMaterial **CWorld::getMaterialSubset(CString indices)
{
	//printf("Indices: \"%s\"\n", indices.c_str());
	CMaterial **ret = new (CMaterial *)[1+indices.length()/2]; //We don't need more
	int i=0;
	while(true)
	{
		int sp = indices.inStr(' ');
		if(sp > 0)
		{
			int n = indices.mid(0,sp).toInt();
			indices= indices.mid(sp+1, indices.length()-sp-1);
			//printf("    Adding %d\n", n);
			*(ret+i) = m_TileMaterials[n];
		}
		else
		{
			//printf("    Adding %d\n", indices.toInt());
			*(ret+i) = m_TileMaterials[indices.toInt()]; //the last index
			break;
		}

		i++;
	}

	return ret;
}
