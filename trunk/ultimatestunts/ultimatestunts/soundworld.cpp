/***************************************************************************
                          soundworld.cpp  -  All the sound data
                             -------------------
    begin                : ma aug 11 2003
    copyright            : (C) 2003 by CJP
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

#include "soundworld.h"
#include "datafile.h"

#include <cstdio>

CSoundWorld::CSoundWorld(const CWorld *world, const CLConfig &conf)
{
	m_World = world;
}

CSoundWorld::~CSoundWorld(){
}

/*
bool CSoundWorld::loadWorld()
{
	return true;
}

void CSoundWorld::unloadWorld()
{
	;
}
*/

bool CSoundWorld::loadObjects()
{
	for(unsigned int i=0; i<m_World->m_MovObjSounds.size(); i++)
	{
		CSndSample *sample = new CSndSample;
		CDataFile f(m_World->m_MovObjSounds[i]);
		printf("   Loading enine sound from %s\n", f.getName().c_str());
		sample->loadFromFile(f.useExtern());

		m_Samples.push_back(sample);
	}

	for(unsigned int i=0; i<m_World->m_MovObjs.size(); i++)
	{
		CMovingObject *mo = m_World->m_MovObjs[i];
		CSoundObj *so = new CSoundObj;
		so->setSample(m_Samples[mo->m_Sounds[0]]); //TODO: load other sounds
		so->setPosVel(CVector(0,0,0), CVector(0,0,0));

		m_Channels.push_back(so);
		m_ObjIDs.push_back(i);
	}

	return true;
}

void CSoundWorld::unloadObjects()
{
	for(unsigned int i=0; i<m_Channels.size(); i++)
		delete m_Channels[i];

	for(unsigned int i=0; i<m_Samples.size(); i++)
		delete m_Samples[i];

	m_Channels.clear();
	m_Samples.clear();
	m_ObjIDs.clear();
}
