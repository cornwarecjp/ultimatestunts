/***************************************************************************
                          loadglt.cpp  -  GLT file loader
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

#include "loadglt.h"

bool loadGLT(const CString &filename, CEditGraphObj &obj)
{
	obj.clear();

	//State variables:
	CVertex state;
		state.pos = CVector(0,0,0);
		state.nor = CVector(0,1,0);
		state.col = CVector(1,1,1);
		state.tex = CVector(0,0,0);
		state.opacity = 1.0;
		state.reflectance = 0.0;
	int texid = -1;
	CString name = "default-name";
	CString LODs = "1234c";

	CDataFile f(filename);

	while(true)
	{
		CString line = f.readl();
		if(line[0] == '\n') break; //EOF

		int sp = line.inStr(' ');
		if(sp > 0)
		{
			CString lhs = line.mid(0, sp);
			CString rhs = line.mid(sp+1, line.length());
			if(lhs == "#Tedit-name")
				name = rhs;
			if(lhs == "Lod")
				LODs = rhs;
			if(lhs == "Texture")
				texid =rhs.toInt();
			if(lhs == "Color")
			{
				state.col = rhs.toVector();
				if(obj.m_Primitives.size()>0) obj.m_Primitives.back().m_ModulationColor = state.col;
			}
			if(lhs == "Opacity")
			{
				state.opacity = rhs.toFloat();
				if(obj.m_Primitives.size()>0) obj.m_Primitives.back().m_Opacity = state.opacity;
			}
			if(lhs == "Reflectance")
			{
				state.reflectance = rhs.toFloat();
				if(obj.m_Primitives.size()>0) obj.m_Primitives.back().m_Reflectance = state.reflectance;
			}
			if(lhs == "Normal")
				state.nor = rhs.toVector();
			if(lhs == "TexCoord")
				state.tex = rhs.toVector();
			if(lhs == "Vertex")
			{
				state.pos = rhs.toVector();
				if(obj.m_Primitives.size()>0) obj.m_Primitives.back().m_Vertex.push_back(state);
			}
			if(lhs == "ReplacementColor")
				if(obj.m_Primitives.size()>0) obj.m_Primitives.back().m_ReplacementColor = rhs.toVector();
			if(lhs == "Emissivity")
				if(obj.m_Primitives.size()>0) obj.m_Primitives.back().m_Emissivity = rhs.toFloat();
			if(lhs == "StaticFriction")
				if(obj.m_Primitives.size()>0) obj.m_Primitives.back().m_StaticFriction = rhs.toFloat();
			if(lhs == "DynamicFriction")
				if(obj.m_Primitives.size()>0) obj.m_Primitives.back().m_DynamicFriction = rhs.toFloat();

			if(lhs == "Index")
				if(obj.m_Primitives.size()>0)
					obj.m_Primitives.back().m_Index.push_back((unsigned int)rhs.toInt());

			if(lhs == "Quads")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::Quads;
				pr.m_LODs = LODs;
				obj.m_Primitives.push_back(pr);
			}
			if(lhs == "Triangles")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::Triangles;
				pr.m_LODs = LODs;
				obj.m_Primitives.push_back(pr);
			}
			if(lhs == "Trianglestrip")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::TriangleStrip;
				pr.m_LODs = LODs;
				obj.m_Primitives.push_back(pr);
			}
			if(lhs == "Quadstrip")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::QuadStrip;
				pr.m_LODs = LODs;
				obj.m_Primitives.push_back(pr);
			}
			if(lhs == "Polygon")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::Polygon;
				pr.m_LODs = LODs;
				obj.m_Primitives.push_back(pr);
			}
			if(lhs == "VertexArray")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::VertexArray;
				pr.m_LODs = LODs;
				obj.m_Primitives.push_back(pr);
			}

			if(lhs == "Notex")
				texid = -1;
			if(lhs == "End")
				name = "default-name";

		}
		else
		{
			CString lhs = line; //easier than doing a find/replace on lhs

			if(lhs == "Quads")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::Quads;
				pr.m_LODs = LODs;
				obj.m_Primitives.push_back(pr);
			}
			if(lhs == "Triangles")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::Triangles;
				pr.m_LODs = LODs;
				obj.m_Primitives.push_back(pr);
			}
			if(lhs == "Trianglestrip")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::TriangleStrip;
				pr.m_LODs = LODs;
				obj.m_Primitives.push_back(pr);
			}
			if(lhs == "Quadstrip")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::QuadStrip;
				pr.m_LODs = LODs;
				obj.m_Primitives.push_back(pr);
			}
			if(lhs == "Polygon")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::Polygon;
				pr.m_LODs = LODs;
				obj.m_Primitives.push_back(pr);
			}
			if(lhs == "VertexArray")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::VertexArray;
				pr.m_LODs = LODs;
				obj.m_Primitives.push_back(pr);
			}

			if(lhs == "Notex")
				texid = -1;
			if(lhs == "End")
				name = "default-name";
		}
	}

	obj.convertToVertexArrays();

	return true;
}
