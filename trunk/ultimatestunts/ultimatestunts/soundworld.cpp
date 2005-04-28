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

CSoundWorld::CSoundWorld(const CLConfig &conf)
{
	m_World = theWorld;
}

CSoundWorld::~CSoundWorld()
{
}

bool CSoundWorld::loadObjects()
{
	vector<CDataObject *> sounds = theWorld->getObjectArray(CDataObject::eSample);
	for(unsigned int i=0; i<sounds.size(); i++)
	{
		//TODO: check ID
		loadObject(sounds[i]->getFilename(), CParamList(), CDataObject::eSample);
	}

	unsigned int crashsoundID = loadObject("cars/generic/crash_nonfatal.wav", CParamList(), CDataObject::eSample);

	for(unsigned int i=0; i<m_World->getNumObjects(CDataObject::eMovingObject); i++)
	{
		const CMovingObject *mo = m_World->getMovingObject(i);

		{ //engine sound
			CSoundObj *so = new CSoundObj(i);
			so->setSample((CSndSample *)getObject(CDataObject::eSample, mo->m_Sounds[0]));
			so->setPosVel(CVector(0,0,0), CVector(0,0,0));
			so->m_SoundType = CSoundObj::eEngine;
			m_Channels.push_back(so);
		}
		{ //skid sound
			CSoundObj *so = new CSoundObj(i);
			so->setSample((CSndSample *)getObject(CDataObject::eSample, mo->m_Sounds[1]));
			so->setPosVel(CVector(0,0,0), CVector(0,0,0));
			so->m_SoundType = CSoundObj::eSkid;
			m_Channels.push_back(so);
		}
		{ //crash sound
			CSoundObj *so = new CSoundObj(i, false);
			so->setSample((CSndSample *)getObject(CDataObject::eSample, crashsoundID));
			so->setPosVel(CVector(0,0,0), CVector(0,0,0));
			so->m_SoundType = CSoundObj::eCrash;
			m_Channels.push_back(so);
		}

		//TODO: load other sounds
	}

	return true;
}

void CSoundWorld::unloadObjects()
{

	for(unsigned int i=0; i<m_Channels.size(); i++)
		delete m_Channels[i];

	m_Channels.clear();

	unloadAll();
}

CDataObject *CSoundWorld::createObject(const CString &filename, const CParamList &plist, CDataObject::eDataType type)
{
	CDataObject *obj = CDataManager::createObject(filename, plist, type);
	if(obj != NULL) return obj;

	if(type == CDataObject::eSample)
		return new CSndSample(this);

	return NULL; //to be overloaded
}
