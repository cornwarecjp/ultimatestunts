/***************************************************************************
                          world.h  -  The world: tiles and cars and other objects
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

#ifndef WORLD_H
#define WORLD_H


/**
  *@author CJP
  */
#include <vector> //STL vector template
namespace std {}
using namespace std;

#include "track.h"
#include "tilemodel.h"
#include "movingobject.h"
#include "objectchoice.h"
#include "collision.h"
#include "chatsystem.h"

#include "datamanager.h"


class CWorld : public CDataManager {
public: 
	CWorld();
	virtual ~CWorld();

	//object helpers
	CMovingObject *getMovingObject(unsigned int ID)
		{return (CMovingObject *)getObject(CDataObject::eMovingObject, ID);}
	const CMovingObject *getMovingObject(unsigned int ID) const
		{return (const CMovingObject *)getObject(CDataObject::eMovingObject, ID);}
	CTileModel *getTileModel(unsigned int ID)
		{return (CTileModel *)getObject(CDataObject::eTileModel, ID);}
	const CTileModel *getTileModel(unsigned int ID) const
		{return (const CTileModel *)getObject(CDataObject::eTileModel, ID);}
	CMaterial *getMaterial(unsigned int ID)
		{return (CMaterial *)getObject(CDataObject::eMaterial, ID);}
	const CMaterial *getMaterial(unsigned int ID) const
		{return (const CMaterial *)getObject(CDataObject::eMaterial, ID);}
	CTrack *getTrack()
		{return (CTrack *)getObject(CDataObject::eTrack, 0);}
	const CTrack *getTrack() const
		{return (const CTrack *)getObject(CDataObject::eTrack, 0);}


	//Collision data
	vector<CCollision> m_Collisions; //is re-filled by the simulation on every frame

	//Rule data
	float m_GameStartTime;

	float m_LastTime, m_Lastdt; //to be set by the game core

	//Interface for sending and receiving messages
	CChatSystem m_ChatSystem;

	//debug
	bool printDebug;
	bool m_Paused;

protected:
	virtual CDataObject *createObject(const CString &filename, const CParamList &plist, CDataObject::eDataType type);
};

extern CWorld *theWorld;

#endif
