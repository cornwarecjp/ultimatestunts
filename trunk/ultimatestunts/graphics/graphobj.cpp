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
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>

#include "vector.h"
#include "graphobj.h"
#include "usmacros.h"
#include "cfile.h"

GLubyte notex_img[1][3];
GLuint no_tex;

CGraphObj::CGraphObj()
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
	//TODO: remove display list when needed
}

bool CGraphObj::loadFromFile(CString filename, CTexture **matarray)
{
	for(int lod=1; lod<5; lod++)
	{
		//printf("Loading graphobj lod=%d\n", lod);

		unsigned int objlist = glGenLists(1);
		glNewList(objlist, GL_COMPILE);

		switch(lod)
		{
			case 1: m_ObjList1 = objlist; break;
			case 2: m_ObjList2 = objlist; break;
			case 3: m_ObjList3 = objlist; break;
			case 4: m_ObjList4 = objlist; break;
		}

		bool texture_isenabled = true;
		setMaterialColor(CVector(1,1,1));
		glBindTexture(GL_TEXTURE_2D, no_tex);


		if(filename.length()==0) //lege string
			{glEndList(); break;}

		bool eof = false;

		CFile f(filename);

		while(!eof)
		{
			CString line = f.readl();
			eof = line[0]=='\n';

			int sp = line.inStr(' ');
			//printf("Spatie op positie %d\n",sp);
			if(sp>0)
			{
				if(line.mid(0, sp)=="Texture")
				{
					line = line.mid(sp+1, line.length());
					int t = (int)line.toFloat();

					//printf("  Using texture %d\n", t);
					CTexture *tex = *(matarray+t);
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
							setMaterialColor(CVector(1,1,1));
						}
					}

					if(texture_isenabled)
						{tex->draw(lod);}
					else //geen texture
					{
						setMaterialColor(tex->getColor());
					}

				}
				if(line.mid(0, sp)=="Color")
				{
					//printf("Color keyword gevonden\n");
					line = line.mid(sp+1, line.length());
					float r = line.toFloat();
					line = line.mid(line.inStr(',')+1, line.length());
					float g = line.toFloat();
					line = line.mid(line.inStr(',')+1, line.length());
					float b = line.toFloat();

					if(texture_isenabled) //oplossing van de kleurenbug
						setMaterialColor(CVector(r,g,b));
				}
				if(line.mid(0, sp)=="Vertex")
				{
					//printf("Vertex keyword gevonden\n");
					line = line.mid(sp+1, line.length());
					float x = line.toFloat();
					line = line.mid(line.inStr(',')+1, line.length());
					float y = line.toFloat();
					line = line.mid(line.inStr(',')+1, line.length());
					float z = line.toFloat();
					glVertex3f(x, y, z);
				}
				if(line.mid(0, sp)=="TexCoord")
				{
					//printf("TexCoord keyword gevonden\n");
					line = line.mid(sp+1, line.length());
					float x = line.toFloat();
					line = line.mid(line.inStr(',')+1, line.length());
					float y = line.toFloat();
					glTexCoord2f(x, y);
				}
				if(line.mid(0, sp)=="Normal")
				{
					//printf("Normal keyword gevonden\n");
					line = line.mid(sp+1, line.length());
					float x = line.toFloat();
					line = line.mid(line.inStr(',')+1, line.length());
					float y = line.toFloat();
					line = line.mid(line.inStr(',')+1, line.length());
					float z = line.toFloat();
					glNormal3f(x, y, z);
				}
				if(line.mid(0, sp)=="Quads")
					glBegin(GL_QUADS);
				if(line.mid(0, sp)=="Triangles")
					glBegin(GL_TRIANGLES);
				if(line.mid(0, sp)=="Trianglestrip")
					glBegin(GL_TRIANGLE_STRIP);
				if(line.mid(0, sp)=="Polygon")
					glBegin(GL_POLYGON);
				if(line.mid(0, sp)=="End")
					glEnd();
				if(line.mid(0, sp)=="Notex")
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
				if(line=="Polygon")
					glBegin(GL_POLYGON);
				if(line=="End")
					glEnd();
				if(line=="Notex")
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

void CGraphObj::draw(int lod) const
{
	//TODO: make this code very fast (it's the main drawing routine)
	//printf("Drawing beautiful openGL graphics...\n");
	switch(lod)
	{
		case 1: glCallList(m_ObjList1); break;
		case 2: glCallList(m_ObjList2); break;
		case 3: glCallList(m_ObjList3); break;
		case 4: glCallList(m_ObjList4); break;
	}
}

void CGraphObj::setMaterialColor(CVector c)
{
	float kleur[] = {c.x, c.y, c.z};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, kleur);
}
