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

CCollisionModel::CCollisionModel(CDataManager *manager) : CDataObject(manager, CDataObject::eCollisionModel)
{}

CCollisionModel::~CCollisionModel(){
}

CString CCollisionModel::getSubset() const
{
	return m_Subset;
}

bool CCollisionModel::load(const CString &idstring)
{
	CDataObject::load(idstring);

	m_Subset = "";
	int pos = m_Filename.inStr(' ');
	if(pos  > 0) //a space exists
	{
		if((unsigned int)pos < m_Filename.length()-1)
			{m_Subset = m_Filename.mid(pos+1, m_Filename.length()-pos-1);}
		m_Filename = m_Filename.mid(0, pos);
	}

	//printf("filename = %s subset = %s\n", m_Filename.c_str(), m_Subset.c_str());

	CMaterial **matarray = getMaterialSubset(m_Subset);

	CDataFile f(m_Filename);

	//Initial bounding volume state
	m_BSphere_r = 0.0;
	m_OBB_min = CVector(0,0,0);
	m_OBB_max = CVector(0,0,0);

	//initial state
	CMaterial *mat = NULL;
	
	//Load faces from file
	ePrimitiveType currentType = None;
	while(true)
	{
		CString line = f.readl();
		if(line[0]=='\n') break;

		int sp = line.inStr(' ');
		if(sp>0)
		{
				CString lhs = line.mid(0, sp);
				CString rhs = line.mid(sp+1);

				if(lhs == "Texture")
				{
					mat = matarray[rhs.toInt()];
				}
				if(lhs == "Notex")
				{
					mat = NULL; //TODO: assign a number for non-texturised materials
				}

				if(lhs == "Lod")
				{
					if(rhs.inStr('c') < 0)
						while(true)
						{
							line = f.readl();
							if(line[0]=='\n') break;
							sp = line.inStr(' ');
							if(sp > 0 && line.mid(0, sp) == "Lod")
							{
								rhs = line.mid(sp+1);
								if(rhs.inStr('c') >= 0)
									break;
							}
						}
				}
				if(lhs=="Vertex" && currentType != None)
				{
					CVector v = rhs.toVector();

					CCollisionFace &theFace = m_Faces.back();
					int index = theFace.size();
					switch(currentType)
					{
					case Triangles:
						if(index == 3)
						{
							m_Faces.push_back(CCollisionFace()); //new face
							m_Faces.back().reverse = false;
							m_Faces.back().material = mat;
						}
						break;
					case Quads:
						if(index == 4)
						{
							m_Faces.push_back(CCollisionFace()); //new face
							m_Faces.back().reverse = false;
							m_Faces.back().material = mat;
						}
						break;
					case Trianglestrip:
						if(index == 3)
						{
							m_Faces.push_back(CCollisionFace()); //new face
							CCollisionFace &newFace = m_Faces.back();
							CCollisionFace &oldFace = m_Faces[m_Faces.size()-2];
							newFace.push_back(oldFace[1]);
							newFace.push_back(oldFace[2]);
							newFace.reverse = !theFace.reverse;
							newFace.material = mat;
						}
						break;
					case Quadstrip:
						if(index == 4)
						{
							m_Faces.push_back(CCollisionFace()); //new face
							CCollisionFace &newFace = m_Faces.back();
							CCollisionFace &oldFace = m_Faces[m_Faces.size()-2];
							newFace.push_back(oldFace[3]);
							newFace.push_back(oldFace[2]);
							newFace.reverse = true;
							newFace.material = mat;
						}
						else if(index == 2) //only for the first quad
						{
							CVector t = theFace[0];
							theFace[0] = theFace[1];
							theFace[1] = t;
						}
						break;
					case Polygon:
					default:
						break;
					}
					(m_Faces.back()).push_back(v);
				}
				if(lhs=="Triangles")
				{
					currentType = Triangles;
					m_Faces.push_back(CCollisionFace());
					m_Faces.back().reverse = false;
					m_Faces.back().material = mat;
				}
				if(lhs=="Quads")
				{
					currentType = Quads;
					m_Faces.push_back(CCollisionFace());
					m_Faces.back().reverse = false;
					m_Faces.back().material = mat;
				}
				if(lhs=="Trianglestrip")
				{
					currentType = Trianglestrip;
					m_Faces.push_back(CCollisionFace());
					m_Faces.back().reverse = false;
					m_Faces.back().material = mat;
				}
				if(lhs=="Quadstrip")
				{
					currentType = Quadstrip;
					m_Faces.push_back(CCollisionFace());
					m_Faces.back().reverse = true;
					m_Faces.back().material = mat;
				}
				if(lhs=="Polygon")
				{
					currentType = Polygon;
					m_Faces.push_back(CCollisionFace());
					m_Faces.back().reverse = false;
					m_Faces.back().material = mat;
				}
		}
		else
		{
			if(line=="Triangles")
			{
				currentType = Triangles;
				m_Faces.push_back(CCollisionFace());
				m_Faces.back().reverse = false;
				m_Faces.back().material = mat;
			}
			if(line=="Quads")
			{
				currentType = Quads;
				m_Faces.push_back(CCollisionFace());
				m_Faces.back().reverse = false;
				m_Faces.back().material = mat;
			}
			if(line=="Trianglestrip")
			{
				currentType = Trianglestrip;
				m_Faces.push_back(CCollisionFace());
				m_Faces.back().reverse = false;
				m_Faces.back().material = mat;
			}
			if(line=="Quadstrip")
			{
				currentType = Quadstrip;
				m_Faces.push_back(CCollisionFace());
				m_Faces.back().reverse = true;
				m_Faces.back().material = mat;
			}
			if(line=="Polygon")
			{
				currentType = Polygon;
				m_Faces.push_back(CCollisionFace());
				m_Faces.back().reverse = false;
				m_Faces.back().material = mat;
			}
		}
	}

	//printf("Loaded %d faces from %s\n", m_Faces.size(), f->getName().c_str());
	delete [] matarray;

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

CMaterial **CCollisionModel::getMaterialSubset(CString indices)
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
			indices = indices.mid(sp+1, indices.length()-sp-1);
			//printf("    Adding %d\n", n);
			*(ret+i) = (CMaterial *)m_DataManager->getObject(CDataObject::eMaterial, n);
		}
		else
		{
			//printf("    Adding %d\n", indices.toInt());
			*(ret+i) = (CMaterial *)m_DataManager->getObject(CDataObject::eMaterial, indices.toInt()); //the last index
			break;
		}

		i++;
	}

	return ret;
}
