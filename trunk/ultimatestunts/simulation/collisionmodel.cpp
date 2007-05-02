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

#include <cstdio>
#include <cmath>
#include "collisionmodel.h"
#include "datafile.h"
#include "glbfile.h"

CCollisionModel::CCollisionModel(CDataManager *manager) : CDataObject(manager, CDataObject::eCollisionModel)
{}

CCollisionModel::~CCollisionModel(){
}

CString CCollisionModel::getSubset() const
{
	return m_Subset;
}

bool CCollisionModel::load(const CString &filename, const CParamList &list)
{
	CDataObject::load(filename, list);

	return loadGLB(filename, list);
}

bool CCollisionModel::loadGLB(const CString &filename, const CParamList &list)
{
	CGLBFile f;
	if(!f.load(filename)) return false;

	m_Subset = m_ParamList.getValue("subset", "");
	vector<CDataObject *> matarray;
	if(m_Subset != "")
		matarray = m_DataManager->getSubset(CDataObject::eMaterial, m_Subset);

	//Initial bounding volume state
	m_BSphere_r = 0.0;
	m_OBB_min = CVector(0,0,0);
	m_OBB_max = CVector(0,0,0);

	//initial state
	CMaterial *mat = NULL;

	for(unsigned int p=0; p < f.m_Primitives.size(); p++)
	{
		CGLBFile::SPrimitive &pr = f.m_Primitives[p];

		if((pr.material.LODs & (16+32)) == 0) continue; //only collision+surface primitives

		//printf("    Processing primitive %s\n", pr.Name.c_str());

		if(pr.material.Texture < 0)
		{
			mat = NULL;
		} //TODO: assign a number for non-texturised materials
		else
		{
			if(pr.material.Texture < int(matarray.size()))
				{mat = (CMaterial *)(matarray[pr.material.Texture]);}
			else
				{mat = NULL;}
		}

		for(unsigned int t=0; t < pr.index.size()/3; t++)
		{
			CVector v1 = pr.vertex[pr.index[3*t]].pos;
			CVector v2 = pr.vertex[pr.index[3*t+1]].pos;
			CVector v3 = pr.vertex[pr.index[3*t+2]].pos;

			m_Faces.push_back(CCollisionFace());
			CCollisionFace &theFace = m_Faces.back();

			theFace.push_back(v1);
			theFace.push_back(v2);
			theFace.push_back(v3);

			theFace.isSurface = (pr.material.LODs & 32) != 0;
			//if(theFace.isSurface) printf("%d is is a surface\n", t);
			theFace.isWater = (pr.material.LODs & 32) != 0 && (pr.material.LODs & 16) != 0;
			if(theFace.isWater) theFace.isSurface = false;

			theFace.material = mat;
			theFace.reverse = false;

			//printf("      Face %d: %d points\n", t, theFace.size());
		}
	}
	
	determineOBVs();
	determinePlaneEquations();

	return true;
}

void CCollisionModel::determineOBVs()
{
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
}

void CCollisionModel::determinePlaneEquations()
{
	//Determine a plane equation for each face
	for(unsigned int i=0; i<m_Faces.size(); i++)
	{
		CCollisionFace &theFace = m_Faces[i];

		//reverse order correction
		if(theFace.reverse)
		{
			unsigned int n = theFace.size();
			for(unsigned int j=0; j < n/2; j++)
			{
				CVector t = theFace[j];
				theFace[j] = theFace[n-j-1];
				theFace[n-j-1] = t;
			}
			theFace.reverse = false;
		}

		//initial:
		theFace.d = 0.0;
		theFace.nor = CVector(0,0,0);

		if(theFace.size() < 3) continue; //not a plane

		//the first point
		CVector p1 = theFace[0];

		//the second point: as far away from 1 as possible
		unsigned int index_2nd = 1;
		CVector p2 = theFace[index_2nd];
		float dist2 = (p2-p1).abs2();
		for(unsigned int j=2; j<theFace.size(); j++)
		{
			float d2 = (theFace[j] - p1).abs2();
			if(d2 > dist2)
				{p2 = theFace[j]; dist2 = d2; index_2nd = j;}
		}

		//the third point: try to maximise cross product
		CVector crosspr;
		float crosspr_abs2 = 0.0;
		bool reverse = false; //2 and 3 in reverse order
		CVector p1p2 = p2 - p1;
		for(unsigned int j=1; j<theFace.size(); j++)
		{
			const CVector &p3 = theFace[j];
			CVector cp = (p3-p1).crossProduct(p1p2);
			float abs2 = cp.abs2();

			if(abs2 > crosspr_abs2)
			{
				crosspr = cp;
				crosspr_abs2 = abs2;
				reverse = (j<index_2nd);
			}
		}

		if(crosspr_abs2 < 0.00001) continue; //too small

		if(reverse) //2 and 3 in reverse order
			theFace.nor = crosspr.normal();
		else
			theFace.nor = -crosspr.normal();

		theFace.d = p1.dotProduct(theFace.nor);

		/*
		printf("  Face %d: p1=(%.2f,%.2f,%.2f) p2=(%.2f,%.2f,%.2f)\n"
			"   p3=(%.2f,%.2f,%.2f)  nor=(%.3f,%.3f,%.3f) d=%.2f\n",
			i,
			//p1.x, p1.y, p1.z,
			//p2.x, p2.y, p2.z,
			//p3.x, p3.y, p3.z,
			theFace[0].x, theFace[0].y, theFace[0].z,
			theFace[1].x, theFace[1].y, theFace[1].z,
			theFace[2].x, theFace[2].y, theFace[2].z,
			theFace.nor.x, theFace.nor.y, theFace.nor.z,
			theFace.d
		);
		*/
	}
}
