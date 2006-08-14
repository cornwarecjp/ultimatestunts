/***************************************************************************
                          loadraw.cpp  -  RAW file loader
                             -------------------
    begin                : wo feb 1 2006
    copyright            : (C) 2006 by CJP
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

#include "datafile.h"

#include "loadraw.h"

bool loadRAW(const CString &filename, CEditGraphObj &obj)
{
	obj.clear();

	CDataFile f(filename);

	while(true)
	{
		CString line = f.readl();
		if(line[0] == '\n') break; //EOF

		if(line[0] > '9') //it is an alphabetical character
		{
				CPrimitive pr;
				pr.m_Name = line;
				pr.m_Texture = -1;
				pr.m_Type = CPrimitive::Triangles;
				pr.m_LODs = "1234c";
				obj.m_Primitives.push_back(pr);
		}
		else
		{
			CVector r1,r2,r3,n1,n2,n3;
			sscanf(line.c_str(), " %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f ",
				&r1.x, &r1.y, &r1.z, &r2.x, &r2.y, &r2.z, &r3.x, &r3.y, &r3.z,
				&n1.x, &n1.y, &n1.z, &n2.x, &n2.y, &n2.z, &n3.x, &n3.y, &n3.z);

			CVertex v1, v2, v3;
			v1.pos = r1;
			v2.pos = r2;
			v3.pos = r3;
			v1.nor = -n1;
			v2.nor = -n2;
			v3.nor = -n3;
			v1.col = v2.col = v3.col = CVector(1,1,1);
			v1.opacity = v2.opacity = v3.opacity = 1.0;
			v1.reflectance = v2.reflectance = v3.reflectance = 0.0;

			obj.m_Primitives[obj.m_Primitives.size()-1].m_Vertex.push_back(v1);
			obj.m_Primitives[obj.m_Primitives.size()-1].m_Vertex.push_back(v2);
			obj.m_Primitives[obj.m_Primitives.size()-1].m_Vertex.push_back(v3);
		}
	}

	obj.convertToVertexArrays();

	return true;
}
