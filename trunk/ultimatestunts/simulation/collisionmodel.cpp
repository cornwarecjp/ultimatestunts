/***************************************************************************
                          collisionmodel.cpp  -  A collision model
                             -------------------
    begin                : wo sep 24 2003
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
#include "collisionmodel.h"

CCollisionModel::CCollisionModel(){
}
CCollisionModel::~CCollisionModel(){
}

bool CCollisionModel::loadFromFile(CFile *f, CString subset, CMaterial **matarray)
{
	//Initial bounding volume state
	m_BSphere_r = 0.0;
	m_OBB_min = CVector(0,0,0);
	m_OBB_max = CVector(0,0,0);

	//Data for graphics etc.
	m_Filename = f->getName();
	m_Subset = subset;

	//Load faces from file
	f->reopen();
	ePrimitiveType currentType = None;
	while(true)
	{
		CString line = f->readl();
		if(line[0]=='\n') break;

		int sp = line.inStr(' ');
		if(sp>0)
		{
				CString lhs = line.mid(0, sp);
				CString rhs = line.mid(sp+1);

				if(lhs=="Vertex" && currentType != None)
				{
					CVector v = rhs.toVector();

					CCollisionFace &theFace = m_Faces[m_Faces.size()-1];
					int index = theFace.size();
					switch(currentType)
					{
					case Triangles:
						if(index == 3)
							m_Faces.push_back(CCollisionFace());
						break;
					case Quads:
						if(index == 4)
							m_Faces.push_back(CCollisionFace());
						break;
					case Trianglestrip:
						if(index == 3)
						{
							m_Faces.push_back(CCollisionFace());
							CCollisionFace &newFace = m_Faces[m_Faces.size()-1];
							newFace.push_back(theFace[2]);
							newFace.push_back(theFace[1]);
						}
						break;
					case Quadstrip:
						if(index == 4)
						{
							m_Faces.push_back(CCollisionFace());
							CCollisionFace &newFace = m_Faces[m_Faces.size()-1];
							CVector v2 = theFace[2];
							CVector v3 = theFace[3];
							theFace[2] = v3;
							theFace[3] = v2;
							newFace.push_back(v2);
							newFace.push_back(v3);
						}
						break;
					case Polygon:
					default:
						;
					}
					(m_Faces[m_Faces.size()-1]).push_back(v);
				}
				if(lhs=="Triangles")
					{currentType = Triangles; m_Faces.push_back(CCollisionFace());}
				if(lhs=="Quads")
					{currentType = Quads; m_Faces.push_back(CCollisionFace());}
				if(lhs=="Trianglestrip")
					{currentType = Trianglestrip; m_Faces.push_back(CCollisionFace());}
				if(lhs=="Quadstrip")
					{currentType = Quadstrip; m_Faces.push_back(CCollisionFace());}
				if(lhs=="Polygon")
					{currentType = Polygon; m_Faces.push_back(CCollisionFace());}
		}
		else
		{
			if(line=="Triangles")
				{currentType = Triangles; m_Faces.push_back(CCollisionFace());}
			if(line=="Quads")
				{currentType = Quads; m_Faces.push_back(CCollisionFace());}
			if(line=="Trianglestrip")
				{currentType = Trianglestrip; m_Faces.push_back(CCollisionFace());}
			if(line=="Quadstrip")
				{currentType = Quadstrip; m_Faces.push_back(CCollisionFace());}
			if(line=="Polygon")
				{currentType = Polygon; m_Faces.push_back(CCollisionFace());}
		}
	}

	//Generating oriented bounding volumes
	for(unsigned int i=0; i<m_Faces.size(); i++)
	{
		CCollisionFace &theFace = m_Faces[i];

		for(unsigned int j=0; j<theFace.size(); j++)
		{
			CVector &v = theFace[j];

			if(v.abs() > m_BSphere_r)
				m_BSphere_r = v.abs();

			if(v.x > m_OBB_max.x) m_OBB_max.x = v.x;
			if(v.y > m_OBB_max.y) m_OBB_max.y = v.y;
			if(v.z > m_OBB_max.z) m_OBB_max.z = v.z;
			if(v.x < m_OBB_min.x) m_OBB_min.x = v.x;
			if(v.y < m_OBB_min.y) m_OBB_min.y = v.y;
			if(v.z < m_OBB_min.z) m_OBB_min.z = v.z;
		}
	}

	return true;
}
