/***************************************************************************
                          movingobject.h  -  description
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

#include "dynamicobject.h"
#include "vector.h"
#include "matrix.h"

class CMovingObject : public CDynamicObject
{
	public:
		CMovingObject();
		~CMovingObject();

		virtual CVector getPosition(){return m_Position;}
		virtual void setPosition(CVector v){m_Position = v;}
		virtual CVector getVelocity(){return m_Velocity;}
		virtual void setVelocity(CVector v){m_Velocity = v;}
		virtual CVector getOrientation(){return m_Orientation;}
		virtual void setOrientation(CVector v);
		virtual CVector getAngularVelocity(){return m_AngularVelocity;}
		virtual void setAngularVelocity(CVector v){m_AngularVelocity = v;}

		virtual const CMatrix &getRotationMatrix(){return m_RotationMatrix;}
		virtual void setRotationMatrix(CMatrix &M);

		virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::movingObject;}

	protected:
		CVector m_Position, m_Velocity, m_Orientation, m_AngularVelocity;
		CMatrix m_RotationMatrix;
};

#endif
