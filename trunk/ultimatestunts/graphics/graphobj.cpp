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
#include "lodtexture.h"


//TODO: remove this (it's unnecessary)
GLubyte notex_img[1][3];
GLuint no_tex;

CGraphObj::CGraphObj(CDataManager *manager, eDataType type) : CDataObject(manager, type)
{
	static bool eerst = true;
	if(eerst)
	{
		//printf("Initializing CGraphObj...\n");
		eerst=false;
		notex_img[0][0] = notex_img[0][1] = notex_img[0][2] = 255;
		glGenTextures(1, &no_tex);
		glBindTexture(GL_TEXTURE_2D, no_tex);
		glTexImage2D( GL_TEXTURE_2D, 0, 3, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, notex_img);

	}
}

CGraphObj::~CGraphObj()
{
}

//bool CGraphObj::loadFromFile(CFile *f, CLODTexture **matarray, int lod_offset)
bool CGraphObj::load(const CString &filename, const CParamList &list)
{
	CDataObject::load(filename, list);
	CDataFile f(filename);
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
		f.reopen();

		unsigned int objlist = glGenLists(1);
		glNewList(objlist, GL_COMPILE);

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

		bool texture_isenabled = true;
		if(lod == 0)
		{
			m_OpacityState = 0.0;
		}
		else
		{
			m_OpacityState = 1.0;
			glBindTexture(GL_TEXTURE_2D, no_tex);
		}
		m_ColorState = CVector(1,1,1);
		setMaterialColor();

		bool eof = false;

		while(!eof)
		{
			CString line = f.readl();
			eof = line[0]=='\n';

			int sp = line.inStr(' ');
			if(sp>0)
			{
				CString lhs = line.mid(0, sp);
				CString rhs = line.mid(sp+1);
				if(lhs == "Lod")
				{
					int lod_eff = lod;
					if(lod == 0) lod_eff = startlod;
					if(rhs.inStr(lod_eff+'0') < 0)
						while(true)
						{
							line = f.readl();
							if(line[0]=='\n') break;
							sp = line.inStr(' ');
							if(sp > 0 && line.mid(0, sp) == "Lod")
							{
								rhs = line.mid(sp+1);
								if(rhs.inStr(lod_eff+'0') >= 0)
									break;
							}
						}
				}
				if(lhs == "Texture" && lod != 0)
				{
					int t = rhs.toInt();

					CLODTexture *tex = (CLODTexture *)matarray[t];
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
							m_ColorState = CVector(1,1,1);
							setMaterialColor();
						}
					}

					if(texture_isenabled)
						{tex->draw(lod);}
					else //geen texture
					{
						m_ColorState = tex->getColor();
						setMaterialColor();
					}

				}
				if(lhs == "Color" && lod != 0)
				{
					m_ColorState = rhs.toVector();
					if(texture_isenabled) //oplossing van de kleurenbug
						setMaterialColor();
				}
				if(lhs == "Opacity" && lod != 0)
				{
					m_OpacityState = rhs.toFloat();
					if(texture_isenabled) //oplossing van de kleurenbug
						setMaterialColor();
				}
				if(lhs == "Reflectance" && lod == 0)
				{
					m_OpacityState = rhs.toFloat();
					setMaterialColor();
				}
				if(lhs == "Vertex")
				{
					CVector v = rhs.toVector();
					glVertex3f(v.x, v.y, v.z);
				}
				if(lhs == "TexCoord" && lod != 0)
				{
					CVector v = rhs.toVector();
					glTexCoord2f(v.x, v.y);
				}
				if(lhs == "Normal")
				{
					//printf("Normal keyword gevonden\n");
					CVector v = rhs.toVector();
					glNormal3f(v.x, v.y, v.z);
				}
				if(lhs == "Quads")
					glBegin(GL_QUADS);
				if(lhs == "Triangles")
					glBegin(GL_TRIANGLES);
				if(lhs == "Trianglestrip")
					glBegin(GL_TRIANGLE_STRIP);
				if(lhs == "Quadstrip")
					glBegin(GL_QUAD_STRIP);
				if(lhs == "Polygon")
					glBegin(GL_POLYGON);
				if(lhs == "End")
					glEnd();
				if(lhs == "Notex" && lod != 0)
					glBindTexture(GL_TEXTURE_2D, no_tex);
			}
			else //no spaces
			{
				if(line=="Quads")
					glBegin(GL_QUADS);
				if(line=="Triangles")
					glBegin(GL_TRIANGLES);
				if(line=="Trianglestrip")
					glBegin(GL_TRIANGLE_STRIP);
				if(line=="Quadstrip")
					glBegin(GL_QUAD_STRIP);
				if(line=="Polygon")
					glBegin(GL_POLYGON);
				if(line=="End")
					glEnd();
				if(line=="Notex" && lod != 0)
					glBindTexture(GL_TEXTURE_2D, no_tex);
			} //if a space exists

		} //while(!eof)

		if(!texture_isenabled)
			glEnable(GL_TEXTURE_2D);

		glEndList();

		//printf("Loading graphobj lod=%d done\n", lod);
	} //for(all lod's)

	return true; //geslaagd
}

void CGraphObj::unload()
{
	CDataObject::unload();

	glDeleteLists(m_ObjListRef, 1);
	glDeleteLists(m_ObjList1, 1);
	glDeleteLists(m_ObjList2, 1);
	glDeleteLists(m_ObjList3, 1);
	glDeleteLists(m_ObjList4, 1);
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
