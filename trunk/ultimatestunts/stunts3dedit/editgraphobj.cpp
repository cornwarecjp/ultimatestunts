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
#include <cstdio> //for sscanf
#include <cmath> //for fabs

#include "3ds.h"

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

bool CEditGraphObj::loadFromFile(CString filename, CLODTexture **matarray)
{
	m_MatArray = matarray;
	clear();

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
			if(lhs == "Opacity")
				state.opacity = rhs.toFloat();
			if(lhs == "Reflectance")
				state.reflectance = rhs.toFloat();
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

	return true;
}

bool CEditGraphObj::import_raw(CString filename, CLODTexture **matarray)
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
				pr.m_LODs = "1234c";
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
			v1.opacity = v2.opacity = v3.opacity = 1.0;
			v1.reflectance = v2.reflectance = v3.reflectance = 0.0;

			m_Primitives[m_Primitives.size()-1].m_Vertex.push_back(v1);
			m_Primitives[m_Primitives.size()-1].m_Vertex.push_back(v2);
			m_Primitives[m_Primitives.size()-1].m_Vertex.push_back(v3);
		}
	}

	return true;
}

bool CEditGraphObj::import_3ds(CString filename, CLODTexture **matarray)
{
	m_MatArray = matarray;
	clear();

	CLoad3DS g_Load3ds;                                     // This is 3DS class.  This should go in a good model class.
	t3DModel g_3DModel;                                     // This holds the 3D Model info that we load in

	g_Load3ds.Import3DS(&g_3DModel, filename.c_str());         // Load our .3DS file into our model structure

	/*
	We won't load the textures, as we're only interested in the
	texture coordinates. Load textures with textures.dat.
	*/
	/*
	for(int i = 0; i < g_3DModel.numOfMaterials; i++)
	{
		// Check to see if there is a file name to load in this material
		if(strlen(g_3DModel.pMaterials[i].strFile) > 0)
		{
			// Use the name of the texture file to load the bitmap, with a texture ID (i).
			// We pass in our global texture array, the name of the texture, and an ID to reference it.
			CreateTexture(g_Texture, g_3DModel.pMaterials[i].strFile, i);
		}


		// Set the texture ID for this material
		g_3DModel.pMaterials[i].texureId = i;
	}
	*/

	// Since we know how many objects our model has, go through each of them.
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		// Make sure we have valid objects just in case. (size() is in the vector class)
		if(g_3DModel.pObject.size() <= 0) break;

		// Get the current object that we are displaying
		t3DObject *pObject = &g_3DModel.pObject[i];

		CPrimitive pr;
		pr.m_Name = pObject->strName;
		pr.m_Type = GL_TRIANGLES;
		pr.m_LODs = "1234c";

		// Check to see if this object has a texture map, if so bind the texture to it.
		if(pObject->bHasTexture)
		{
			pr.m_Texture = -1;
		}
		else
		{
			//default to no texture even if there is a texture
			pr.m_Texture = -1;
			//pr.m_Texture = pObject->materialID;
		}

		// Go through all of the faces (polygons) of the object and draw them
		for(int j = 0; j < pObject->numOfFaces; j++)
		{
			// Go through each corner of the triangle and draw it.
			for(int whichVertex = 0; whichVertex < 3; whichVertex++)
			{
				// Get the index for each point of the face
				int index = pObject->pFaces[j].vertIndex[whichVertex];

				CVertex vt;
				vt.col = CVector(1,1,1);
				vt.opacity = 1.0;
				vt.reflectance = 0.0;

				// Give OpenGL the normal for this vertex.
				vt.nor = CVector(pObject->pNormals[ index ].x, pObject->pNormals[ index ].y, pObject->pNormals[ index ].z);

				// If the object has a texture associated with it, give it a texture coordinate.
				if(pObject->bHasTexture)
				{
					// Make sure there was a UVW map applied to the object or else it won't have tex coords.
					if(pObject->pTexVerts)
						vt.tex = CVector(pObject->pTexVerts[ index ].x, pObject->pTexVerts[ index ].y, 0.0);
				}
				else
				{

					// Make sure there is a valid material/color assigned to this object.
					// You should always at least assign a material color to an object,
					// but just in case we want to check the size of the material list.
					// if the size is at least one, and the material ID != -1,
					// then we have a valid material.
					if(g_3DModel.pMaterials.size() && pObject->materialID >= 0)
					{
						// Get and set the color that the object is, since it must not have a texture
						BYTE *pColor = g_3DModel.pMaterials[pObject->materialID].color;

						// Assign the current color to this model
						vt.col.x = (float)(pColor[0]) / 255.0;
						vt.col.y = (float)(pColor[1]) / 255.0;
						vt.col.z = (float)(pColor[2]) / 255.0;
					}
				}

				// Pass in the current vertex of the object (Corner of current face)
				vt.pos = CVector(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
				pr.m_Vertex.push_back(vt);
			}

		}

		m_Primitives.push_back(pr);
	}

	return true;
}

void CEditGraphObj::merge(const CEditGraphObj &obj, const CString &lods)
{
	//merge 2 objects
	//if objects have equal names, then they are placed together
	//and the new one gets the suffix of the LODS

	//calculate CG position
	CVector CG1, CG2;
	unsigned int num1 = 0, num2 = 0;
	for(unsigned int i=0; i < obj.m_Primitives.size(); i++)
		for(unsigned int j=0; j < obj.m_Primitives[i].m_Vertex.size(); j++)
		{
			CG2 += obj.m_Primitives[i].m_Vertex[j].pos;
			num2++;
		}
	CG2 /= num2;

	for(unsigned int i=0; i < m_Primitives.size(); i++)
		for(unsigned int j=0; j < m_Primitives[i].m_Vertex.size(); j++)
		{
			CG1 += m_Primitives[i].m_Vertex[j].pos;
			num1++;
		}
	CG1 /= num1;

	for(unsigned int i=0; i < obj.m_Primitives.size(); i++)
	{
		CPrimitive thePrimitive = obj.m_Primitives[i];

		//Correct CG position
		thePrimitive.m_LODs = lods;
		for(unsigned int j=0; j < thePrimitive.m_Vertex.size(); j++)
			thePrimitive.m_Vertex[j].pos += CG1 - CG2;

		int match = -1;
		for(unsigned int j=0; j < m_Primitives.size(); j++)
			if(m_Primitives[j].m_Name == thePrimitive.m_Name)
				{match = j; break;}

		if(match < 0) //no match: just add it to the end
		{
			m_Primitives.push_back(thePrimitive);
			continue;
		}

		//there is a match
		thePrimitive.m_Name = thePrimitive.m_Name + "_" + lods;

		//set the color
		for(unsigned int j=0; j < thePrimitive.m_Vertex.size(); j++)
		{
			thePrimitive.m_Vertex[j].col = m_Primitives[match].m_Vertex[0].col;
			thePrimitive.m_Vertex[j].opacity = m_Primitives[match].m_Vertex[0].opacity;
			thePrimitive.m_Vertex[j].reflectance = m_Primitives[match].m_Vertex[0].reflectance;
		}
		
		m_Primitives.insert(m_Primitives.begin() + match, thePrimitive);
		
	}
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
	f.writel("Opacity 1");
	f.writel("Reflectance 0");

	//State variables:
	CVertex state;
		state.pos = CVector(0,0,0);
		state.nor = CVector(0,1,0);
		state.col = CVector(1,1,1);
		state.opacity = 1.0;
		state.reflectance = 0.0;
		state.tex = CVector(0,0,0);
	int texid = -1;
	CString name = "default-name";
	CString LODs = "1234c";

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

			//rewrite the entire state (set the current state totally invalid)
			//TODO: more efficient
			texid = -2;
			state.col = state.nor = state.tex = CVector(2,2,2);
			state.opacity = state.reflectance = 2;
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

			//opacity
			if(fabs(vt.opacity - state.opacity) > 0.01)
			{
				f.writel(CString("Opacity ")+vt.opacity);
				state.opacity = vt.opacity;
			}

			//reflectance
			if(fabs(vt.reflectance - state.reflectance) > 0.01)
			{
				f.writel(CString("Reflectance ")+vt.reflectance);
				state.reflectance = vt.reflectance;
			}

			//normal
			if((vt.nor-state.nor).abs2() > 0.0001)
			{
				f.writel(CString("Normal ")+vt.nor.x+", "+vt.nor.y+", "+vt.nor.z);
				state.nor = vt.nor;
			}

			//texcoord
			if((vt.tex-state.tex).abs2() > 0.001)
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

void CEditGraphObj::render(const CString &visibleLODs)
{
	//delete existing list
	if(isRendered)
	{
		printf("Deleting old model...\n");
		glDeleteLists(m_ObjList1, 1);
		glDeleteLists(m_ObjListRef, 1);
	}

	printf("Generating model...\n");

	//Normal model
	m_ObjList1 = m_ObjList2 = m_ObjList3 = m_ObjList4 = glGenLists(1);
	glNewList(m_ObjList1, GL_COMPILE);

	for(unsigned int i=0; i<m_Primitives.size(); i++)
	{
		CPrimitive &pr = m_Primitives[i];

		CString LODs = pr.m_LODs;
		bool doit = false;
		for(unsigned int j=0; j < LODs.length(); j++)
			if(visibleLODs.inStr( (LODs[j]) ) >= 0)
				{doit = true; break;}
		if(!doit) continue;
		
		//printf("test begin\n");
		if(pr.m_Texture < 0)
			{glDisable(GL_TEXTURE_2D);}
		else
		{
			glEnable(GL_TEXTURE_2D);
			m_MatArray[pr.m_Texture]->draw(1);
		}
		//printf("test eind\n");

		glBegin(pr.m_Type);
		for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
		{
			CVertex &vt = pr.m_Vertex[j];
			CVector &nor = vt.nor;
			glNormal3f(nor.x, nor.y, nor.z);
			m_OpacityState = vt.opacity;
			m_ColorState = vt.col;
			setMaterialColor();
			CVector &tex = vt.tex;
			glTexCoord2f(tex.x, tex.y);
			CVector &pos = vt.pos;
			glVertex3f(pos.x, pos.y, pos.z);
		}
		glEnd();
	}

	glEndList();

	//Reflection model
	m_ObjListRef = glGenLists(1);
	glNewList(m_ObjListRef, GL_COMPILE);

	m_ColorState = CVector(1,1,1);
	for(unsigned int i=0; i<m_Primitives.size(); i++)
	{
		CPrimitive &pr = m_Primitives[i];
		if(pr.m_LODs.inStr('1') < 0) continue; //reflection model = LOD 1

		glBegin(pr.m_Type);
		for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
		{
			CVertex &vt = pr.m_Vertex[j];
			CVector &nor = vt.nor;
			glNormal3f(nor.x, nor.y, nor.z);
			m_OpacityState = vt.reflectance;
			setMaterialColor();
			CVector &pos = vt.pos;
			glVertex3f(pos.x, pos.y, pos.z);
		}
		glEnd();
	}

	glEndList();

	
	isRendered = true;
}
