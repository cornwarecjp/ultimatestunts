/***************************************************************************
                          bound.cpp  -  Plane-based collision model
                             -------------------
    begin                : vr jan 24 2003
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

#include "bound.h"

CBound::CBound(){
}
CBound::~CBound(){
}

bool CBound::loadFromFile(CFile *f, CString subset, CMaterial **matarray)
{
	CCollisionModel::loadFromFile(f, subset, matarray);

	m_Points.clear();
	for(unsigned int i=0; i < m_Faces.size()-2; i++)
		for(unsigned int j=i+1; j < m_Faces.size()-1; j++)
		{
			CCollisionFace &fi = m_Faces[i];
			CCollisionFace &fj = m_Faces[j];
			float dotpr = fi.nor.dotProduct(fj.nor);
			if(dotpr > -0.99 && dotpr < 0.99) //not parallel
			{
				CVector dr = fi.nor.crossProduct(fj.nor);
				CVector r0 = ( (fi.d*fi.nor + fj.d*fj.nor) - dotpr*(fj.d*fi.nor + fi.d*fj.nor) ) / (1.0 - dotpr*dotpr);
				for(unsigned int k=j+1; k < m_Faces.size(); k++)
				{
					CCollisionFace &fk = m_Faces[k];
					dotpr = dr.dotProduct(fk.nor);
					if(dotpr > 0.01 || dotpr < -0.01) //not parallel
					{
						CVector p = r0 + ((fk.d - r0.dotProduct(fk.nor)) / dotpr)*dr;

						bool inside = true;
						for(unsigned int l=0; l < m_Faces.size(); l++)
							if(l != i && l != j && l != k &&
								p.dotProduct(m_Faces[l].nor) > m_Faces[l].d)
								{inside = false; break;}

						if(inside)
							m_Points.push_back(p);
					}
				}
			}
		}

	return true;
}
