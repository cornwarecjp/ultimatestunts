/***************************************************************************
                          collisiondata.cpp  -  data describing collision events
                             -------------------
    begin                : di sep 23 2003
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

#include <stdio.h>

#include "collisiondata.h"
#include "world.h"

CCollisionData::CCollisionData(const CWorld *w)
{
	m_World = w;
}

CCollisionData::~CCollisionData()
{
}

void CCollisionData::calculateCollisions()
{
	m_Events.clear();

	for(unsigned int i=0; i<m_World->m_MovObjs.size(); i++)
	{
		m_Events.push_back(CColEvents());
		m_Events[i].isHit = false;
	}

	//Moving object to moving object collisions
	for(unsigned int i=0; i < m_World->m_MovObjs.size()-1; i++)
		for(unsigned int j=i+1; j < m_World->m_MovObjs.size(); j++)
			ObjObjTest(i,j);
}

void CCollisionData::ObjObjTest(int n1, int n2)
{
	CMovingObject *o1 = m_World->m_MovObjs[n1];
	CMovingObject *o2 = m_World->m_MovObjs[n2];
	CVector pos1 = o1->getPosition();
	CVector pos2 = o2->getPosition();

	//TODO: per body collision
	//for(unsigned int i=0; i<o1->m_Bodies.size(); i++)
	//	for(unsigned int j=0; j<o2->m_Bodies.size(); j++)
	int i=0, j=0;
		{
			CVector p1 = pos1 + o1->m_Bodies[i].m_Position;
			CVector p2 = pos2 + o2->m_Bodies[j].m_Position;
			const CBound *b1 = m_World->m_MovObjBounds[o1->m_Bodies[i].m_Body];
			const CBound *b2 = m_World->m_MovObjBounds[o2->m_Bodies[i].m_Body];
			if(sphereTest(p1, b1, p2, b2))
			{
				printf("Collision between %d and %d\n", n1, n2);
				CCollision c1, c2;

				c1.nor = p2 - p1;
				c1.nor.normalise();
				c2.nor = -c1.nor;

				c1.pos = b1->m_BSphere_r * c1.nor;
				c2.pos = b2->m_BSphere_r * c2.nor;

				c1.mat1 = c1.mat2 = NULL;
				c2.mat1 = c1.mat1;
				c2.mat2 = c1.mat2;

				c1.body = i;
				c2.body = j;

				//position correction
				float dr = 0.5 * (b1->m_BSphere_r + b2->m_BSphere_r - (p2-p1).abs());
				c1.dr = c1.nor * -dr;
				c2.dr = c2.nor * -dr;

				//determine momentum transfer
				{
					CVector v1 = o1->getVelocity().component(c1.nor);
					CVector v2 = o2->getVelocity().component(c1.nor);
					float m1 = o2->m_Mass;
					float m2 = o2->m_Mass;
					CVector vcg = (m1*v1 + m2*v2)/(m1 + m2);
					c1.dp = 2.0*m1*(vcg - v1);

					c2.dp = -c1.dp;
				}

				m_Events[n1].isHit = true;
				m_Events[n1].push_back(c1);
				m_Events[n2].isHit = true;
				m_Events[n2].push_back(c2);
			}
		}
}

bool CCollisionData::sphereTest(const CVector &p1, const CBound *b1, const CVector &p2, const CBound *b2)
{
	float abs2 = (p2-p1).abs2();
	float rsum = b1->m_BSphere_r + b2->m_BSphere_r;
	return abs2 < (rsum * rsum);
}
