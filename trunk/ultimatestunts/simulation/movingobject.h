/***************************************************************************
                          movingobject.h  -  Moving object class
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

#ifndef MOVINGOBJECT_H
#define MOVINGOBJECT_H


/**
  *@author CJP
  */
#include "message.h"
#include "dataobject.h"

#include "vector.h"
#include "matrix.h"
#include "body.h"

class CPhysics;

class CMovingObject : public CDataObject, CMessage
{
public:
	CMovingObject(CDataManager *manager);
	~CMovingObject();

	virtual void resetBodyPositions(CVector pos, const CMatrix &ori)=0; //should be called after setting object position, orientation

	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::movingObject;}

	//Update: physics simulation
	virtual void update(CPhysics *simulator, float dt);

	const CVector &getCameraPos() const
		{return m_CameraPos;}

	vector<CBody> m_Bodies; //The object bodies
	vector<unsigned int> m_Sounds; //The object sounds

	//For network transfer & other stuff
	virtual CBinBuffer &getData(CBinBuffer &b) const;       // returns class data as binbuffer
	virtual bool setData(const CBinBuffer &b);   // rebuild class data from binbuffer

	virtual bool hasChanged();

	CMessage *m_InputData;
protected:
	CVector m_CameraPos; //the relative position of the inside camera
};

#endif
