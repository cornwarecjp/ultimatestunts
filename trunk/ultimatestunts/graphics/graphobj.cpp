/***************************************************************************
                          graphobj.cpp  -  Graphics geometry object
                             -------------------
    begin                : Tue Apr 23 2002
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
#include <cstdio>
#include <cstdlib>

#include <GL/gl.h>


#include "glextensions.h"
#include "vector.h"
#include "usmacros.h"
#include "lconfig.h"
#include "graphicworld.h"
#include "datafile.h"
#include "glbfile.h"
#include "lodtexture.h"

#include "graphobj.h"

CGraphObj::CGraphObj(CDataManager *manager, eDataType type) : CDataObject(manager, type)
{
}

CGraphObj::~CGraphObj()
{
}

bool CGraphObj::load(const CString &filename, const CParamList &list)
{
	CDataObject::load(filename, list);

	CGLBFile f;
	if(!f.load(filename)) return false;

	CString subset = m_ParamList.getValue("subset", "");
	vector<CDataObject *> matarray = m_DataManager->getSubset(CDataObject::eMaterial, subset);

	//the primitives
	for(unsigned int p=0; p < f.m_Primitives.size(); p++)
	{
		CGLBFile::SPrimitive &pr1 = f.m_Primitives[p];

		m_Primitives.push_back(SPrimitive());
		SPrimitive &pr2 = m_Primitives.back();

		//set some properties
		pr2.LODs = pr1.LODs;
		pr2.opacity = pr1.Opacity;
		pr2.reflectance = pr1.Reflectance;
		pr2.emissivity = pr1.Emissivity;

		//Color and texture:
		//default:
		pr2.texture[0] = pr2.texture[1] = pr2.texture[2] = pr2.texture[3] = 0;
		pr2.color[0] = pr2.color[1] = pr2.color[2] = pr2.color[3] = pr1.ModulationColor;

		//add texture
		if(pr1.Texture >= 0)
		{
			CLODTexture *tex = (CLODTexture *)matarray[pr1.Texture];

			for(unsigned int lod=0; lod < 3; lod++)
			{
				if(tex->getSizeX(lod+1) <=4 || tex->getSizeY(lod+1) <= 4)
				{
					//TODO: blend modulation & replacement color
					pr2.color[lod] = pr1.ReplacementColor;
				}
				else
				{
					pr2.texture[lod] = tex->getTextureID(lod+1);
				}
			}
			
		}

		//add vertices
		pr2.numVertices = pr1.vertex.size();
		GLfloat *vertexptr = new GLfloat[8*pr1.vertex.size()];
		for(unsigned int v=0; v < pr1.vertex.size(); v++)
		{
			CGLBFile::SVertex &vt = pr1.vertex[v];

			GLfloat *offset = vertexptr + 8*v;

			//GL_T2F_N3F_V3F array format:
			*(offset  ) = vt.tex.x;
			*(offset+1) = vt.tex.y;
			
			*(offset+2) = vt.nor.x;
			*(offset+3) = vt.nor.y;
			*(offset+4) = vt.nor.z;
			
			*(offset+5) = vt.pos.x;
			*(offset+6) = vt.pos.y;
			*(offset+7) = vt.pos.z;
		}
		pr2.vertex = vertexptr;

		//add indices
		pr2.numIndices = pr1.index.size();
		GLuint *indexptr = new GLuint[pr1.index.size()];
		for(unsigned int i=0; i < pr1.index.size(); i++)
			*(indexptr + i) = pr1.index[i];
		pr2.index = indexptr;
	}

	return true;
}

void CGraphObj::unload()
{
	if(!isLoaded()) return;

	CDataObject::unload();

	//primitives:
	for(unsigned int i=0; i < m_Primitives.size(); i++)
		unloadPrimitive(m_Primitives[i]);
	m_Primitives.clear();
}

void CGraphObj::unloadPrimitive(SPrimitive &pr)
{
	delete [] (GLfloat *)pr.vertex;
	delete [] (GLuint *)pr.index;
}

bool tex_enabled;

void CGraphObj::draw(int lod) const
{
	//TODO: make this code very fast (it's the main drawing routine)

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	tex_enabled = true;

	for(unsigned int p=0; p < m_Primitives.size(); p++)
	{
		const SPrimitive &pr = m_Primitives[p];

		//LOD testing:
		if(lod == 4 && (pr.LODs & 8) == 0) continue;
		if(lod == 3 && (pr.LODs & 4) == 0) continue;
		if(lod == 2 && (pr.LODs & 2) == 0) continue;
		if(lod == 1 && (pr.LODs & 1) == 0) continue;
		if(lod == 0 && (pr.LODs & 1) == 0) continue;

		if(lod == 0) //reflection
		{
			float kleur[] = {1, 1, 1, pr.reflectance};
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, kleur);
		}
		else
		{
			float kleur[] = {pr.color[lod-1].x, pr.color[lod-1].y, pr.color[lod-1].z, pr.opacity};
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, kleur);

			if(pr.texture[lod-1] == 0)
			{
				if(tex_enabled)
				{
					glDisable(GL_TEXTURE_2D);
					tex_enabled = false;
				}
			}
			else
			{
				if(!tex_enabled)
				{
					glEnable(GL_TEXTURE_2D);
					tex_enabled = true;
				}
				glBindTexture(GL_TEXTURE_2D, pr.texture[lod-1]);
			}
		}

		drawArray(pr.vertex, pr.index, pr.numVertices, pr.numIndices);
	}

	if(!tex_enabled)
		glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
}

/*
void CGraphObj::drawTriangles(void *vertex, void *index, unsigned int numV, unsigned int numI) const
{
	GLfloat *vertexptr = (GLfloat *)vertex;
	GLuint *indexptr = (GLuint *)index;

	glBegin(GL_TRIANGLES);

	for(unsigned int i=0; i < numI; i++)
	{
		GLuint index = *(indexptr + i);

		GLfloat *offset = vertexptr + 8*index;

		//GL_T2F_N3F_V3F array format:

		if(tex_enabled)
			glTexCoord2fv(offset);

		glNormal3fv(offset + 2);
		glVertex3fv(offset + 5);
	}

	glEnd();
}
*/

void CGraphObj::drawArray(void *vertex, void *index, unsigned int numV, unsigned int numI) const
{
	CGLExtensions ext;
	if(ext.hasCompiledVertexArray) // && numV < numI) //it was just a try
	{
		glInterleavedArrays(GL_T2F_N3F_V3F, 0, vertex);
		ext.glLockArrays(0, numV); //is the num argument correct??
		glDrawElements(GL_TRIANGLES, numI, GL_UNSIGNED_INT, index);
		ext.glUnlockArrays();
	}
	else
	{
		glInterleavedArrays(GL_T2F_N3F_V3F, 0, vertex);
		glDrawElements(GL_TRIANGLES, numI, GL_UNSIGNED_INT, index);
	}
}

/*
void CGraphObj::drawDispList(int lod) const
{
	switch(lod)
	{
		case 0: glCallList(m_ObjListRef); break;
		case 1: glCallList(m_ObjList1); break;
		case 2: glCallList(m_ObjList2); break;
		case 3: glCallList(m_ObjList3); break;
		case 4: glCallList(m_ObjList4); break;
	}
}

void CGraphObj::generateDispLists(int lod_offset)
{
	int startlod = lod_offset + 1;
	int stoplod = lod_offset + 4;
	if(startlod < 1) startlod = 1;
	if(startlod > 4) startlod = 4;
	if(stoplod < 1) stoplod = 1;
	if(stoplod > 4) stoplod = 4;

	for(int lod=0; lod<5; lod++)
	{
		//printf("Loading graphobj lod=%d\n", lod);

		unsigned int objlist = glGenLists(1);
		glNewList(objlist, GL_COMPILE);
		//fprintf(stderr, "Loaded object @ %d\n", objlist);

		switch(lod)
		{
			case 0: m_ObjListRef = objlist; break;
			case 1: m_ObjList1 = objlist; break;
			case 2: m_ObjList2 = objlist; break;
			case 3: m_ObjList3 = objlist; break;
			case 4: m_ObjList4 = objlist; break;
		}

		if( (lod > 0 && lod < startlod) || lod > stoplod)
		{
			glEndList();
			continue;
		}

		drawLod(lod);

		glEndList();
	}
}
*/

