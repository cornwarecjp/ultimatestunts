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
#include "editgraphobj.h"
#include "cfile.h"

CEditGraphObj::CEditGraphObj() : CGraphObj()
{
	unsigned int objlist = glGenLists(1);
	m_ObjList1 = m_ObjList2 = m_ObjList3 = m_ObjList4 = objlist;
}

CEditGraphObj::~CEditGraphObj()
{
	//TODO: delete list
}

bool CEditGraphObj::loadFromFile(CString filename, CTexture **matarray)
{
	m_MatArray = matarray;

	//State variables:
	CVertex state;
		state.pos = CVector(0,0,0);
		state.nor = CVector(0,1,0);
		state.col = CVector(1,1,1);
		state.tex = CVector(0,0,0);
	int texid = -1;
	CString name = "default-name";

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
			if(lhs == "Texture")
				texid =rhs.toInt();
			if(lhs == "Color")
			{
				float r = rhs.toFloat();
				rhs = rhs.mid(rhs.inStr(',')+1, rhs.length());
				float g = rhs.toFloat();
				rhs = rhs.mid(rhs.inStr(',')+1, rhs.length());
				float b = rhs.toFloat();
				state.col = CVector(r,g,b);
			}
			if(lhs == "Normal")
			{
				float x = rhs.toFloat();
				rhs = rhs.mid(rhs.inStr(',')+1, rhs.length());
				float y = rhs.toFloat();
				rhs = rhs.mid(rhs.inStr(',')+1, rhs.length());
				float z = rhs.toFloat();
				state.nor = CVector(x,y,z);
			}
			if(lhs == "TexCoord")
			{
				float x = rhs.toFloat();
				rhs = rhs.mid(rhs.inStr(',')+1, rhs.length());
				float y = rhs.toFloat();
				state.tex = CVector(x,y,0.0);
			}
			if(lhs == "Vertex")
			{
				float x = rhs.toFloat();
				rhs = rhs.mid(rhs.inStr(',')+1, rhs.length());
				float y = rhs.toFloat();
				rhs = rhs.mid(rhs.inStr(',')+1, rhs.length());
				float z = rhs.toFloat();
				state.pos = CVector(x,y,z);

				m_Primitives[m_Primitives.size()-1].m_Vertex.push_back(state);
			}

			if(lhs == "Quads")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_QUADS;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Triangles")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_TRIANGLES;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Trianglestrip")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_TRIANGLE_STRIP;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Polygon")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_POLYGON;
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
				m_Primitives.push_back(pr);
			}
			if(lhs == "Triangles")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_TRIANGLES;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Trianglestrip")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_TRIANGLE_STRIP;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Polygon")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = GL_POLYGON;
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

void CEditGraphObj::render()
{
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
}
