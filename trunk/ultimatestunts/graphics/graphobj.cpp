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
//GLfloat white[] = {1,1,1};

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

int CGraphObj::loadFromFile(CString filename, CTexObj *texarray, int lod)
{
	m_ObjList = glGenLists(1);
	glNewList(m_ObjList, GL_COMPILE);

	bool texture_isenabled = true;
  setMaterialColor(CVector(1,1,1));
  glBindTexture(GL_TEXTURE_2D, no_tex);


	if(filename.length()==0) //lege string
		{glEndList(); return 0;}

	bool eof = false;

		CFile f(filename);

		while(!eof)
		{
			CString line = f.readl();
			eof = line[0]=='\n';

			int sp = line.instr(' ');
			//printf("Spatie op positie %d\n",sp);
			if(sp>0)
			{
				if(line.mid(0, sp)=="Texture")
				{
					line = line.mid(sp+1, line.length());
					int t = (int)line.toFloat();

					if((texarray+t)->getSizeX(lod) <=4 || (texarray+t)->getSizeY(lod) <= 4)
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
  					{(texarray+t)->draw(lod);}
					else //geen texture
  					{setMaterialColor((texarray+t)->getColor());}

				}
				if(line.mid(0, sp)=="Color")
				{
					//printf("Color keyword gevonden\n");
					line = line.mid(sp+1, line.length());
					float r = line.toFloat();
					line = line.mid(line.instr(',')+1, line.length());
					float g = line.toFloat();
					line = line.mid(line.instr(',')+1, line.length());
					float b = line.toFloat();

          if(texture_isenabled) //oplossing van de kleurenbug
    			  setMaterialColor(CVector(r,g,b));
				}
				if(line.mid(0, sp)=="Vertex")
				{
					//printf("Vertex keyword gevonden\n");
					line = line.mid(sp+1, line.length());
					float x = line.toFloat();
					line = line.mid(line.instr(',')+1, line.length());
					float y = line.toFloat();
					line = line.mid(line.instr(',')+1, line.length());
					float z = line.toFloat();
					glVertex3f(x, y, z);
				}
				if(line.mid(0, sp)=="TexCoord")
				{
					//printf("TexCoord keyword gevonden\n");
					line = line.mid(sp+1, line.length());
					float x = line.toFloat();
					line = line.mid(line.instr(',')+1, line.length());
					float y = line.toFloat();
					glTexCoord2f(x, y);
				}
				if(line.mid(0, sp)=="Normal")
				{
					//printf("Normal keyword gevonden\n");
					line = line.mid(sp+1, line.length());
					float x = line.toFloat();
					line = line.mid(line.instr(',')+1, line.length());
					float y = line.toFloat();
					line = line.mid(line.instr(',')+1, line.length());
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
			else
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
			}

		}

	if(!texture_isenabled)
			glEnable(GL_TEXTURE_2D);



	glEndList();

	//printf ("Vorm-Object is geladen.\n");
	return 0; //geslaagd
}

void CGraphObj::draw()
{
  glCallList(m_ObjList);
}

void CGraphObj::setMaterialColor(CVector c)
{
  float kleur[] = {c.x, c.y, c.z};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, kleur);
}
