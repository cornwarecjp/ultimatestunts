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

#include "vector.h"
#include "graphobj.h"
#include "usmacros.h"
#include "lconfig.h"
#include "graphicworld.h"
#include "datafile.h"
#include "glbfile.h"
#include "lodtexture.h"


CGraphObj::CGraphObj(CDataManager *manager, eDataType type) : CDataObject(manager, type)
{
	m_ObjListRef = m_ObjList1 = m_ObjList2 = m_ObjList3 = m_ObjList4 = 0;
}

CGraphObj::~CGraphObj()
{
}

//bool CGraphObj::loadFromFile(CFile *f, CLODTexture **matarray, int lod_offset)
bool CGraphObj::load(const CString &filename, const CParamList &list)
{
	CDataObject::load(filename, list);

	return loadGLB(filename, list);
}

bool CGraphObj::loadGLB(const CString &filename, const CParamList &list)
{
	CGLBFile f;
	if(!f.load(filename)) return false;

	CString subset = m_ParamList.getValue("subset", "");
	vector<CDataObject *> matarray = m_DataManager->getSubset(CDataObject::eMaterial, subset);
	int lod_offset = m_ParamList.getValue("lodoffset", "0").toInt();

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

		//the flag in the LOD byte
		unsigned int lodflag = 0;
		if(lod == 0)
			{lodflag = 1;}
		else
			{lodflag = 1 << (lod-1);}


		//Initial state
		bool texture_isenabled = true;
		if(lod == 0)
		{
			m_OpacityState = 0.0;
		}
		else
		{
			m_OpacityState = 1.0;
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		m_ColorState = CVector(1,1,1);
		setMaterialColor();

		//the primitives
		for(unsigned int p=0; p < f.m_Primitives.size(); p++)
		{
			CGLBFile::SPrimitive &pr = f.m_Primitives[p];
			
			if(!(pr.LODs & lodflag))
				continue; //not in this LOD

			//set colors etc.
			//TODO: texture replacement color
			if(lod == 0)
			{
				m_OpacityState = pr.Reflectance;
			}
			else
			{
				m_ColorState = pr.ModulationColor;
				m_OpacityState = pr.Opacity;

				if(pr.Texture < 0)
				{
					texture_isenabled = false;
					glDisable(GL_TEXTURE_2D);
				}
				else
				{
					CLODTexture *tex = (CLODTexture *)matarray[pr.Texture];
					if(tex->getSizeX(lod) <=4 || tex->getSizeY(lod) <= 4)
					{
						if(texture_isenabled)
						{
							glDisable(GL_TEXTURE_2D);
							texture_isenabled=false;
						}
					}
					else
					{
						if(!texture_isenabled)
						{
							glEnable(GL_TEXTURE_2D);
							texture_isenabled=true;
							//m_ColorState = CVector(1,1,1); //TODO
						}
					}

					if(texture_isenabled)
						{tex->draw(lod);}
					else //geen texture
					{
						m_ColorState = tex->getColor(); //TODO: replace by texture replacement color
					}
				}
			}
			setMaterialColor();
			
			glBegin(GL_TRIANGLES);

			for(unsigned int i=0; i < pr.index.size(); i++)
			{
				CGLBFile::SVertex v = pr.vertex[pr.index[i]];
				glNormal3f(v.nor.x, v.nor.y, v.nor.z);
				if(texture_isenabled)
					glTexCoord2f(v.tex.x, v.tex.y);
				glVertex3f(v.pos.x, v.pos.y, v.pos.z);
			}

			glEnd();
		}

		if(!texture_isenabled)
			glEnable(GL_TEXTURE_2D);

		glEndList();
	}

	return true;
}

void CGraphObj::unload()
{
	if(!isLoaded()) return;

	CDataObject::unload();

	//fprintf(stderr, "unload object %d\n", m_ObjListRef);
	//fprintf(stderr, "unload object %d\n", m_ObjList1);
	//fprintf(stderr, "unload object %d\n", m_ObjList2);
	//fprintf(stderr, "unload object %d\n", m_ObjList3);
	//fprintf(stderr, "unload object %d\n", m_ObjList4);

	glDeleteLists(m_ObjListRef, 1);
	glDeleteLists(m_ObjList1, 1);
	glDeleteLists(m_ObjList2, 1);
	glDeleteLists(m_ObjList3, 1);
	glDeleteLists(m_ObjList4, 1);
	m_ObjListRef = m_ObjList1 = m_ObjList2 = m_ObjList3 = m_ObjList4 = 0;
}

void CGraphObj::draw(int lod) const
{
	//TODO: make this code very fast (it's the main drawing routine)
	switch(lod)
	{
		case 0: glCallList(m_ObjListRef); break;
		case 1: glCallList(m_ObjList1); break;
		case 2: glCallList(m_ObjList2); break;
		case 3: glCallList(m_ObjList3); break;
		case 4: glCallList(m_ObjList4); break;
	}
}

void CGraphObj::setMaterialColor()
{
	float kleur[] = {m_ColorState.x, m_ColorState.y, m_ColorState.z, m_OpacityState};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, kleur);
}
