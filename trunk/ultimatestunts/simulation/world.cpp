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

CWorld::CWorld(){
}

CWorld::~CWorld(){
	unloadTrack();
	unloadMovObjs();
}

bool CWorld::loadTrack(CString filename)
{
	printf("The world is being loaded from %s\n", filename.c_str());

	//First: load materials/textures
	//for example:
	CMaterial *m = createMaterial();
	m->loadFromFile("no_file.rgb", 256, 256); //Only useful for textures
	//TODO: set friction coefficients
	m_TileMaterials.push_back(m);
	
	//Second: loading collision (and graphics) data
	//For example:
	CShape *b = createShape();
	m_TileShapes.push_back(b);
	//TODO: use loadFromFile

	//Third: initialising track array
	l=10; w=10; h=2; //example
	for(int x=0; x<l; x++)
	for(int z=0; z<w; z++)
	for(int y=0; y<h; y++)
	{
		CTile t;
		t.m_Shape = m_TileShapes[0]; //for example
		t.m_R = t.m_Z = 0; //TODO: load from file (of course)
		m_Track.push_back(t);

	}

	int s = m_Track.size();
	printf("Added tiles: total %d tiles\n", s);

	return true;
}

void CWorld::unloadTrack()
{
	printf("Unloading track...\n");

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

/*
int CWorld::addMovingObject(CObjectChoice c)
{
	//future: selecting, using c

	CMovingObject *m = new CCar;
	m_MovObjs.push_back(m);

	int s = m_MovObjs.size();
	printf("Added car: total %d moving objects\n", s);

	return s - 1;
}
*/
bool CWorld::loadMovObjs(CString filename)
{
	printf("Loading moving objects...\n");

	//Temporary solution: nr of objs put in string
	int num = filename.toInt();

	//First: load materials/textures
	//for example:
	CMaterial *m = createMaterial();
	m->loadFromFile("no_file.rgb", 256, 256); //Only useful for textures
	//TODO: set friction coefficients
	m_MovObjMaterials.push_back(m);

	//Second: loading collision (and graphics) data
	//For example:
	CBound *b = createBound();
	m_MovObjBounds.push_back(b);
	//TODO: use loadFromFile

	//Third: initialising car array
	for(int i=0; i<num; i++)
	{
		CMovingObject *m = new CCar; //future: selecting, using the file
		m->m_Bound = m_MovObjBounds[0]; //for example
		m_MovObjs.push_back(m);

		int s = m_MovObjs.size();
		printf("Added car: total %d moving objects\n", s);
	}

		return true;
}

void CWorld::unloadMovObjs()
{
	printf("Unloading moving objects...\n");

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

CShape *CWorld::createShape()
{return new CShape;}

CBound *CWorld::createBound()
{return new CBound;}

CMaterial *CWorld::createMaterial()
{return new CMaterial;}
