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
#include <cstdio>

#include "world.h"
#include "car.h"
#include "datafile.h"

CWorld *theWorld = NULL;

CWorld::CWorld()
{
	printDebug = false;
	m_Paused = true;

	m_ODEWorld = dWorldCreate();

	/*
	I know that this defies the law of gravity, but,
	you see, I never studied law.

	Bugs Bunny
	*/
	dWorldSetGravity(m_ODEWorld, 0.0, -9.81, 0.0);
	m_ContactGroup = dJointGroupCreate(0);

	theWorld = this;
}

CWorld::~CWorld(){
	unloadTrack();
	unloadMovObjs();

	dJointGroupDestroy(m_ContactGroup);
	dWorldDestroy(m_ODEWorld);

	theWorld = NULL;
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
		float mu = 1.0;
		line = tfile.readl();
		if(line == "END") break;
		int pos = line.inStr("scale=");
		if(pos  > 0) //scale is specified
		{
			if((unsigned int)pos < line.length()-6)
				{mul = line.mid(pos+6, line.length()-pos-6).toInt();}
			//TODO: check for different y-direction mul
		}

		pos = line.inStr("mu=");
		if(pos  > 0) //mu is specified
		{
			if((unsigned int)pos < line.length()-3)
				{mu = line.mid(pos+3, line.length()-pos-3).toFloat();}
		}

		pos = line.inStr(' ');
		if(pos  > 0) //there is a space
			line = line.mid(0, pos);

		CMaterial *m = new CMaterial;
		m->m_Filename = line;
		m->m_Mul =  mul;
		m->m_Mu = mu;
		m_TileMaterials.push_back(m);
	}
	printf("\n   Loaded %d materials\n\n", m_TileMaterials.size());

	while(tfile.readl() != "BEGIN"); //Begin of background section
	m_BackgroundFilename = tfile.readl();
	m_EnvMapFilename = tfile.readl();

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

		CTileModel *b = new CTileModel;

		CMaterial **subset = getMaterialSubset(texture_indices);
		{
			CDataFile f(line);
			b->loadFromFile(&f, texture_indices, subset);
		}
		delete [] subset;

		b->m_isStart = tile_flags.inStr('s') >= 0;
		b->m_isFinish = tile_flags.inStr('f') >= 0;
		b->m_Time = 1.0;

		m_TileModels.push_back(b);
	}
	printf("\n   Loaded %d tile models\n\n", m_TileModels.size());

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

void CWorld::unloadTrack()
{
	printf("   Unloading track...\n");

	//The array containing the track
	m_Track.clear();


	if(m_TileModels.size() > 0)
	{
		//The CTileModel objects themselves
		for(unsigned int i=0; i<m_TileModels.size(); i++)
			delete m_TileModels[i];

		//The array containing them
		m_TileModels.clear();
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

int CWorld::getMovObjBoundID(const CString &filename)
{
	int ret = -1;
	for(unsigned int i=0; i < m_MovObjBounds.size(); i++)
		if(getShortName(m_MovObjBounds[i]->m_Filename) == filename)
		{
			ret = i;
			break;
		}

	if(ret < 0) //we have to load it
	{
		CBound *body = new CBound;
		CDataFile fbody(filename);
		body->loadFromFile(&fbody, "", NULL);

		//temporary way to find the wheels
		if(filename.inStr("wheel") >= 0)
			body->setCylinder(true);

		m_MovObjBounds.push_back(body);
		ret = m_MovObjBounds.size() - 1;
	}

	return ret;
}

int CWorld::getMovObjSoundID(const CString &filename)
{
	int ret = -1;
	for(unsigned int i=0; i < m_MovObjSounds.size(); i++)
		if(m_MovObjSounds[i] == filename)
		{
			ret = i;
			break;
		}

	if(ret < 0) //we have to load it
	{
		m_MovObjSounds.push_back(filename);
		ret = m_MovObjSounds.size() - 1;
	}

	return ret;
}

bool CWorld::loadMovingObject(const CObjectChoice &oc)
{
	CMovingObject *m = new CCar(oc.m_Filename);
	m_MovObjs.push_back(m);
	printf("   Added car %s\n", oc.m_Filename.c_str());

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
	CMaterial **ret = new CMaterial *[1+indices.length()/2]; //We don't need more
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
