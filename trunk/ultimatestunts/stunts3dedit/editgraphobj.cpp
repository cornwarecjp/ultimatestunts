/***************************************************************************
                          editgraphobj.cpp  -  An editable graphics object
                             -------------------
    begin                : wo apr 16 2003
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

#include <GL/gl.h>
#include <stdio.h> //for sscanf
#include "editgraphobj.h"
#include "cfile.h"

CEditGraphObj::CEditGraphObj() : CGraphObj()
{
	isRendered = false;
}

CEditGraphObj::~CEditGraphObj()
{
	if(isRendered)
		glDeleteLists(m_ObjList1, 1);
}

bool CEditGraphObj::loadFromFile(CString filename, CTexture **matarray)
{
	m_MatArray = matarray;
	clear();

	//State variables:
	CVertex state;
		state.pos = CVector(0,0,0);
		state.nor = CVector(0,1,0);
		state.col = CVector(1,1,1);
		state.tex = CVector(0,0,0);
	int texid = -1;
	CString name = "default-name";
	CString LODs = "1234s";

	CFile f(filename);

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
				state.col = rhs.toVector();
			if(lhs == "Normal")
				state.nor = rhs.toVector();
			if(lhs == "TexCoord")
				state.tex = rhs.toVector();
			if(lhs == "Vertex")
			{
				state.pos = rhs.toVector();
				m_Primitives[m_Primitives.size()-1].m_Vertex.push_back(state);
			}

			if(lhs == "Quads")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_QUADS;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Triangles")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_TRIANGLES;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Trianglestrip")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_TRIANGLE_STRIP;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Quadstrip")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_QUAD_STRIP;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Polygon")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_POLYGON;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
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
				pr.m_Type = GL_QUADS;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Triangles")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_TRIANGLES;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Trianglestrip")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_TRIANGLE_STRIP;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Quadstrip")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_QUAD_STRIP;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Polygon")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_POLYGON;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}

			if(lhs == "Notex")
				texid = -1;
			if(lhs == "End")
				name = "default-name";
		}
	}

	render();
	return true;
}

bool CEditGraphObj::import_raw(CString filename, CTexture **matarray)
{
	m_MatArray = matarray;
	clear();

	CFile f(filename);

	while(true)
	{
		CString line = f.readl();
		if(line[0] == '\n') break; //EOF

		if(line[0] > '9') //it is an alphabetical character
		{
				CPrimitive pr;
				pr.m_Name = line;
				pr.m_Texture = -1;
				pr.m_Type = GL_TRIANGLES;
				pr.m_LODs = "1234s";
				m_Primitives.push_back(pr);
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

			m_Primitives[m_Primitives.size()-1].m_Vertex.push_back(v1);
			m_Primitives[m_Primitives.size()-1].m_Vertex.push_back(v2);
			m_Primitives[m_Primitives.size()-1].m_Vertex.push_back(v3);
		}
	}

	render();
	return true;
}

void CEditGraphObj::clear()
{
	m_Primitives.clear();
}

void CEditGraphObj::saveToFile(CString filename)
{
	CFile f(filename, true);
	f.writel("#CornWare UltimateStunts graphics file");
	f.writel("#created by the stunts3dedit editor");
	f.writel("");
	f.writel("Normal 0,1,0");
	f.writel("Color 1,1,1");

	//State variables:
	CVertex state;
		state.pos = CVector(0,0,0);
		state.nor = CVector(0,1,0);
		state.col = CVector(1,1,1);
		state.tex = CVector(0,0,0);
	int texid = -1;
	CString name = "default-name";
	CString LODs = "1234s";

	for(unsigned int i=0; i<m_Primitives.size(); i++)
	{
		CPrimitive &pr = m_Primitives[i];

		f.writel(""); //two empty lines
		f.writel("");
		if(!(pr.m_Name == name))
		{
			name = pr.m_Name;
			f.writel("#Tedit-name " + name);
		}
		if(!(pr.m_LODs == LODs))
		{
			LODs = pr.m_LODs;
			f.writel("Lod " + LODs);
		}
		if(!(pr.m_Texture == texid))
		{
			texid = pr.m_Texture;
			if(texid < 0)
				f.writel("Notex");
			else
				f.writel(CString("Texture ") + texid);
		}

		int numvert = 1; //spacing property
		switch(pr.m_Type)
		{
			case GL_QUADS:
				f.writel("Quads"); numvert = 4; break;
			case GL_TRIANGLES:
				f.writel("Triangles"); numvert = 3; break;
			case GL_TRIANGLE_STRIP:
				f.writel("Trianglestrip"); break;
			case GL_QUAD_STRIP:
				f.writel("Quadstrip"); break;
			case GL_POLYGON:
				f.writel("Polygon"); break;
		}

		for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
		{
			if((j % numvert) == 0)
				f.writel(""); //empty line

			CVertex &vt = pr.m_Vertex[j];

			//color
			if((vt.col-state.col).abs2() > 0.0001)
			{
				f.writel(CString("Color ")+vt.col.x+", "+vt.col.y+", "+vt.col.z);
				state.col = vt.col;
			}

			//normal
			if((vt.nor-state.nor).abs2() > 0.0001)
			{
				f.writel(CString("Normal ")+vt.nor.x+", "+vt.nor.y+", "+vt.nor.z);
				state.nor = vt.nor;
			}

			//texcoord
			if((vt.tex-state.tex).abs2() > 0.0001)
			{
				f.writel(CString("TexCoord ")+vt.tex.x+", "+vt.tex.y);
				state.tex = vt.tex;
			}

			//vertex
			f.writel(CString("Vertex ")+vt.pos.x+", "+vt.pos.y+", "+vt.pos.z);
		}

		f.writel("End");
	}

}

void CEditGraphObj::render()
{
	//delete existing list
	if(isRendered)
		{printf("Deleting old model...\n"); glDeleteLists(m_ObjList1, 1);}

	printf("Generating model...\n");
	m_ObjList1 = m_ObjList2 = m_ObjList3 = m_ObjList4 = glGenLists(1);
	glNewList(m_ObjList1, GL_COMPILE);

	for(unsigned int i=0; i<m_Primitives.size(); i++)
	{
		CPrimitive &pr = m_Primitives[i];
		if(pr.m_Texture < 0)
			{glDisable(GL_TEXTURE_2D);}
		else
		{
			glEnable(GL_TEXTURE_2D);
			m_MatArray[pr.m_Texture]->draw(1);
		}

		glBegin(pr.m_Type);
		for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
		{
			CVertex &vt = pr.m_Vertex[j];
			CVector &nor = vt.nor;
			glNormal3f(nor.x, nor.y, nor.z);
			CVector &col = vt.col;
			GLfloat kleur[] = {col.x, col.y, col.z, 1.0};
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, kleur);
			CVector &tex = vt.tex;
			glTexCoord2f(tex.x, tex.y);
			CVector &pos = vt.pos;
			glVertex3f(pos.x, pos.y, pos.z);
		}
		glEnd();
	}

	glEndList();

	isRendered = true;
}
