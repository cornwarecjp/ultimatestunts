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
#include "lw.h"
#include "glbfile.h"

#include "editgraphobj.h"
#include "datafile.h"

CEditGraphObj::CEditGraphObj()
{
	m_ObjList = m_ObjListRef = 0;
	isRendered = false;
}

CEditGraphObj::~CEditGraphObj()
{
	if(isRendered)
	{
		glDeleteLists(m_ObjList, 1);
		glDeleteLists(m_ObjListRef, 1);
	}
}

bool CEditGraphObj::loadGLTFile(CString filename)
{
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
				if(m_Primitives.size()>0) m_Primitives.back().m_ModulationColor = state.col;
			}
			if(lhs == "Opacity")
			{
				state.opacity = rhs.toFloat();
				if(m_Primitives.size()>0) m_Primitives.back().m_Opacity = state.opacity;
			}
			if(lhs == "Reflectance")
			{
				state.reflectance = rhs.toFloat();
				if(m_Primitives.size()>0) m_Primitives.back().m_Reflectance = state.reflectance;
			}
			if(lhs == "Normal")
				state.nor = rhs.toVector();
			if(lhs == "TexCoord")
				state.tex = rhs.toVector();
			if(lhs == "Vertex")
			{
				state.pos = rhs.toVector();
				if(m_Primitives.size()>0) m_Primitives.back().m_Vertex.push_back(state);
			}
			if(lhs == "ReplacementColor")
				if(m_Primitives.size()>0) m_Primitives.back().m_ReplacementColor = rhs.toVector();
			if(lhs == "Emissivity")
				if(m_Primitives.size()>0) m_Primitives.back().m_Emissivity = rhs.toFloat();
			if(lhs == "StaticFriction")
				if(m_Primitives.size()>0) m_Primitives.back().m_StaticFriction = rhs.toFloat();
			if(lhs == "DynamicFriction")
				if(m_Primitives.size()>0) m_Primitives.back().m_DynamicFriction = rhs.toFloat();

			if(lhs == "Index")
				if(m_Primitives.size()>0)
					m_Primitives.back().m_Index.push_back((unsigned int)rhs.toInt());

			if(lhs == "Quads")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::Quads;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Triangles")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::Triangles;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Trianglestrip")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::TriangleStrip;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Quadstrip")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::QuadStrip;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Polygon")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::Polygon;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "VertexArray")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::VertexArray;
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
				pr.m_Type = CPrimitive::Quads;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Triangles")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::Triangles;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Trianglestrip")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::TriangleStrip;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Quadstrip")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::QuadStrip;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "Polygon")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::Polygon;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}
			if(lhs == "VertexArray")
			{
				CPrimitive pr;
				pr.m_Name = name;
				pr.m_Texture = texid;
				pr.m_Type = CPrimitive::VertexArray;
				pr.m_LODs = LODs;
				m_Primitives.push_back(pr);
			}

			if(lhs == "Notex")
				texid = -1;
			if(lhs == "End")
				name = "default-name";
		}
	}

	convertToVertexArrays();

	return true;
}

bool CEditGraphObj::loadGLBFile(CString filename)
{
	CGLBFile f;
	if(!f.load(filename)) return false;

	clear();
	for(unsigned int p=0; p < f.m_Primitives.size(); p++)
	{
		m_Primitives.push_back(CPrimitive());
		CPrimitive &pr2 = m_Primitives.back();
		CGLBFile::SPrimitive &pr1 = f.m_Primitives[p];

		pr2.m_Type = CPrimitive::VertexArray;
		pr2.m_DynamicFriction = pr1.DynamicFriction;
		pr2.m_Emissivity = pr1.Emissivity;
		if(pr1.LODs & 1) pr2.m_LODs += '1';
		if(pr1.LODs & 2) pr2.m_LODs += '2';
		if(pr1.LODs & 4) pr2.m_LODs += '3';
		if(pr1.LODs & 8) pr2.m_LODs += '4';
		if(pr1.LODs & 16) pr2.m_LODs += 'c';
		if(pr1.LODs & 32) pr2.m_LODs += 's';
		pr2.m_ModulationColor = pr1.ModulationColor;
		pr2.m_Name = pr1.Name;
		pr2.m_Opacity = pr1.Opacity;
		pr2.m_Reflectance = pr1.Reflectance;
		pr2.m_ReplacementColor = pr1.ReplacementColor;
		pr2.m_StaticFriction = pr1.StaticFriction;
		pr2.m_Texture = pr1.Texture;

		for(unsigned int v=0; v < pr1.vertex.size(); v++)
		{
			CVertex vt2;
			CGLBFile::SVertex &vt1 = pr1.vertex[v];

			vt2.pos = vt1.pos;
			vt2.nor = vt1.nor;
			vt2.tex = vt1.tex;

			pr2.m_Vertex.push_back(vt2);
		}

		pr2.m_Index = pr1.index;
	}

	return true;
}

bool CEditGraphObj::loadRAWFile(CString filename)
{
	clear();

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

	convertToVertexArrays();

	return true;
}

bool CEditGraphObj::load3DSFile(CString filename)
{
	CDataFile f(filename);
	CString fn2 = f.useExtern();
	
	clear();

	CLoad3DS g_Load3ds;                                     // This is 3DS class.  This should go in a good model class.
	t3DModel g_3DModel;                                     // This holds the 3D Model info that we load in

	g_Load3ds.Import3DS(&g_3DModel, fn2.c_str());         // Load our .3DS file into our model structure

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
		pr.m_Type = CPrimitive::VertexArray;
		pr.m_LODs = "1234c";
		pr.m_Emissivity = 0.0;
		pr.m_Opacity = 1.0;
		pr.m_Reflectance = 0.0;
		pr.m_ModulationColor = CVector(1,1,1);
		pr.m_ReplacementColor = CVector(1,1,1);
		pr.m_DynamicFriction = 1.0;
		pr.m_StaticFriction = 1.0;

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

		//add the vertex array
		for(int j=0; j < pObject->numOfVerts; j++)
		{
			CVertex vt;

			vt.nor = CVector(pObject->pNormals[j].x, pObject->pNormals[j].y, pObject->pNormals[j].z);
			vt.pos = CVector(pObject->pVerts[j].x, pObject->pVerts[j].y, pObject->pVerts[j].z);
			if(pObject->bHasTexture)
			{
				// Make sure there was a UVW map applied to the object or else it won't have tex coords.
				if(pObject->pTexVerts)
					vt.tex = CVector(pObject->pTexVerts[j].x, pObject->pTexVerts[j].y, 0.0);
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
					pr.m_ModulationColor.x = (float)(pColor[0]) / 255.0;
					pr.m_ModulationColor.y = (float)(pColor[1]) / 255.0;
					pr.m_ModulationColor.z = (float)(pColor[2]) / 255.0;
				}
			}

			pr.m_Vertex.push_back(vt);
		}
		
		// Go through all of the faces (polygons) of the object and draw them
		for(int j = 0; j < pObject->numOfFaces; j++)
		{
			// Go through each corner of the triangle and draw it.
			for(int whichVertex = 0; whichVertex < 3; whichVertex++)
			{
				// Get the index for each point of the face
				unsigned int index = pObject->pFaces[j].vertIndex[whichVertex];

				pr.m_Index.push_back(index);
			}

		}

		m_Primitives.push_back(pr);
	}

	return true;
}

bool CEditGraphObj::loadLWOFile(CString filename)
{
	CDataFile f(filename);
	CString fn2 = f.useExtern();

	if(!lw_is_lwobject(fn2.c_str())) return false;

	clear();

	lwObject *lwo = lw_object_read(fn2.c_str());
	if(lwo == NULL) return false;

	printf("Loaded all data from LWO file\n");
	
	//first, create a primitive for every material
	for(int i=0; i < lwo->material_cnt; i++)
	{
		m_Primitives.push_back(CPrimitive());
		CPrimitive &pr = m_Primitives.back();

		pr.m_Name = lwo->material[i].name;
		pr.m_Type = CPrimitive::VertexArray;
		pr.m_LODs = "1234c";
		pr.m_Emissivity = 0.0;
		pr.m_Opacity = 1.0;
		pr.m_Reflectance = 0.0;
		pr.m_ModulationColor = CVector(
			lwo->material[i].r,
			lwo->material[i].g,
			lwo->material[i].b
			);
		pr.m_ReplacementColor = pr.m_ModulationColor;
		pr.m_DynamicFriction = 1.0;
		pr.m_StaticFriction = 1.0;

		//copy the entire vertex array to every primitive
		for(int j=0; j < lwo->vertex_cnt; j++)
		{
			CVertex v;
			v.nor = CVector(1,0,0);
			v.pos = CVector(
				lwo->vertex[3*j+0],
				lwo->vertex[3*j+1],
				lwo->vertex[3*j+2]
				);
			pr.m_Vertex.push_back(v);
		}
	}

	printf("Created all primitives\n");

	//copy faces to the primitives
	for(int i=0; i < lwo->face_cnt; i++)
	{
		const lwFace *face = lwo->face+i;

		/* ignore faces with less than 3 points */
		if (face->index_cnt < 3)
			continue;

		CPrimitive &thePrimitive = m_Primitives[face->material];

		for(int j=0; j < face->index_cnt; j++)
		{
			thePrimitive.m_Index.push_back(face->index[j]);

			//TODO: update texture coordinates:
			/*
			thePrimitive.m_Vertex[face->index[j]].tex = CVector(
				
				);
			*/
		}
	}

	printf("Added all faces\n");

	//TODO: optimise the result (remove unused vertices and empty primitives)

	lw_object_free(lwo);

	printf("Loading was succesful\n");
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

void CEditGraphObj::saveGLTFile(const CString &filename)
{
	CDataFile f(filename, true);
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

		//special code for vertex arrays
		if(pr.m_Type == CPrimitive::VertexArray)
		{
			f.writel("VertexArray");
			f.writel(CString("Color ") + pr.m_ModulationColor);
			f.writel(CString("Opacity ") + pr.m_Opacity);
			f.writel(CString("ReplacementColor ") + pr.m_ReplacementColor);
			f.writel(CString("Reflectance ") + pr.m_Reflectance);
			f.writel(CString("Emissivity ") + pr.m_Emissivity);
			f.writel(CString("DynamicFriction ") + pr.m_DynamicFriction);
			f.writel(CString("StaticFriction ") + pr.m_StaticFriction);
			
			for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
			{
				CVertex &vt = pr.m_Vertex[j];

				//normal
				if((vt.nor-state.nor).abs2() > 0.0001)
				{
					f.writel(CString("Normal ")+vt.nor.x+", "+vt.nor.y+", "+vt.nor.z);
					state.nor = vt.nor;
				}

				//texcoord
				if((vt.tex-state.tex).abs2() > 0.000001)
				{
					f.writel(CString("TexCoord ")+vt.tex.x+", "+vt.tex.y);
					state.tex = vt.tex;
				}

				//vertex
				f.writel(CString("Vertex ")+vt.pos.x+", "+vt.pos.y+", "+vt.pos.z);
			}
			for(unsigned int j=0; j<pr.m_Index.size(); j++)
				f.writel(CString("Index ") + pr.m_Index[j]);
			f.writel("End");
			continue;
		}
		
		int numvert = 1; //spacing property
		switch(pr.m_Type)
		{
			case CPrimitive::Quads:
				f.writel("Quads"); numvert = 4; break;
			case CPrimitive::Triangles:
				f.writel("Triangles"); numvert = 3; break;
			case CPrimitive::TriangleStrip:
				f.writel("Trianglestrip"); break;
			case CPrimitive::QuadStrip:
				f.writel("Quadstrip"); break;
			case CPrimitive::Polygon:
				f.writel("Polygon"); break;
			case CPrimitive::VertexArray:
				f.writel("#Vertex array not yet supported");
				break;
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
			if((vt.tex-state.tex).abs2() > 0.000001)
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

void CEditGraphObj::saveGLBFile(const CString &filename)
{
	CGLBFile f;

	for(unsigned int p=0; p < m_Primitives.size(); p++)
	{
		CPrimitive &pr1 = m_Primitives[p];
		f.m_Primitives.push_back(CGLBFile::SPrimitive());
		CGLBFile::SPrimitive &pr2 = f.m_Primitives.back();

		pr2.DynamicFriction = pr1.m_DynamicFriction;
		pr2.Emissivity = pr1.m_Emissivity;
		pr2.LODs = 0;
		if(pr1.m_LODs.inStr('1') >= 0) pr2.LODs += 1;
		if(pr1.m_LODs.inStr('2') >= 0) pr2.LODs += 2;
		if(pr1.m_LODs.inStr('3') >= 0) pr2.LODs += 4;
		if(pr1.m_LODs.inStr('4') >= 0) pr2.LODs += 8;
		if(pr1.m_LODs.inStr('c') >= 0) pr2.LODs += 16;
		if(pr1.m_LODs.inStr('s') >= 0) pr2.LODs += 32;
		pr2.ModulationColor = pr1.m_ModulationColor;
		pr2.Name = pr1.m_Name;
		pr2.Opacity = pr1.m_Opacity;
		pr2.Reflectance = pr1.m_Reflectance;
		pr2.ReplacementColor = pr1.m_ReplacementColor;
		pr2.StaticFriction = pr1.m_StaticFriction;
		pr2.Texture = pr1.m_Texture;

		for(unsigned int v=0; v < pr1.m_Vertex.size(); v++)
		{
			CGLBFile::SVertex vt2;
			CVertex &vt1 = pr1.m_Vertex[v];

			vt2.pos = vt1.pos;
			vt2.nor = vt1.nor;
			vt2.tex = vt1.tex;

			pr2.vertex.push_back(vt2);
		}

		pr2.index = pr1.m_Index;
	}

	f.save(filename);
}

void CEditGraphObj::convertToVertexArrays()
{
	for(unsigned int p=0; p < m_Primitives.size(); p++)
	{
		CPrimitive &pr = m_Primitives[p];

		pr.m_ModulationColor = pr.m_Vertex[0].col;
		pr.m_Opacity = pr.m_Vertex[0].opacity;
		pr.m_Reflectance = pr.m_Vertex[0].reflectance;
		if(pr.m_Texture >= 0)
		{
			pr.m_ReplacementColor = m_MatArray[pr.m_Texture]->getColor();
			pr.m_ReplacementColor.x *= pr.m_ModulationColor.x;
			pr.m_ReplacementColor.y *= pr.m_ModulationColor.y;
			pr.m_ReplacementColor.z *= pr.m_ModulationColor.z;
		}
		else
			{pr.m_ReplacementColor = pr.m_ModulationColor;}
		pr.m_Emissivity = 0;
		pr.m_StaticFriction = 1.0;
		pr.m_DynamicFriction = 1.0;

		unsigned int maxIndex = pr.m_Vertex.size() - 1;
		unsigned int writeIndex = 0;
		unsigned int readIndex = 0;

		switch(pr.m_Type)
		{
		case CPrimitive::VertexArray:
			writeIndex = maxIndex+1;
			break;

		case CPrimitive::Triangles:
			for(readIndex=0; readIndex <= maxIndex; readIndex++)
			{
				CVertex v = pr.m_Vertex[readIndex];
				pr.m_Index.push_back(findOrAdd(pr, v, writeIndex));
			}
			break;
		
		case CPrimitive::Quads:
			for(readIndex=0; readIndex <= maxIndex; readIndex+= 4)
			{
				CVertex v1 = pr.m_Vertex[readIndex];
				CVertex v2 = pr.m_Vertex[readIndex+1];
				CVertex v3 = pr.m_Vertex[readIndex+2];
				CVertex v4 = pr.m_Vertex[readIndex+3];

				unsigned int index1 = findOrAdd(pr, v1, writeIndex);
				unsigned int index2 = findOrAdd(pr, v2, writeIndex);
				unsigned int index3 = findOrAdd(pr, v3, writeIndex);
				unsigned int index4 = findOrAdd(pr, v4, writeIndex);

				pr.m_Index.push_back(index1);
				pr.m_Index.push_back(index2);
				pr.m_Index.push_back(index3);
				pr.m_Index.push_back(index1);
				pr.m_Index.push_back(index3);
				pr.m_Index.push_back(index4);
			}
			break;

		case CPrimitive::TriangleStrip:
		case CPrimitive::QuadStrip:
		{
			if(maxIndex < 2) break;
			unsigned int index1 = 0;
			unsigned int index2 = 1;
			writeIndex = 2;
			bool even = false;
			for(readIndex=2; readIndex <= maxIndex; readIndex++)
			{
				CVertex v3 = pr.m_Vertex[readIndex];

				unsigned int index3 = findOrAdd(pr, v3, writeIndex);

				if(even)
				{
					pr.m_Index.push_back(index3);
					pr.m_Index.push_back(index2);
					pr.m_Index.push_back(index1);
				}
				else
				{
					pr.m_Index.push_back(index1);
					pr.m_Index.push_back(index2);
					pr.m_Index.push_back(index3);
				}

				index1 = index2;
				index2 = index3;
				even = !even;
			}
			break;
		}
		
		case CPrimitive::Polygon:
		{
			if(maxIndex < 2) break;
			unsigned int index1 = 1;
			writeIndex = 2;
			for(readIndex=2; readIndex <= maxIndex; readIndex++)
			{
				CVertex v2 = pr.m_Vertex[readIndex];

				unsigned int index2 = findOrAdd(pr, v2, writeIndex);

				pr.m_Index.push_back(0);
				pr.m_Index.push_back(index1);
				pr.m_Index.push_back(index2);

				index1 = index2;
			}
			break;
		}

		}

		pr.m_Vertex.resize(writeIndex);
		printf("Object %s reduced from %d to %d vertices\n",
			pr.m_Name.c_str(), maxIndex+1, writeIndex);

		pr.m_Type = CPrimitive::VertexArray;
	}
}

unsigned int CEditGraphObj::findOrAdd(CPrimitive &pr, const CVertex &v, unsigned int &writeIndex)
{
	int found = -1;
	for(unsigned int i=0; i < writeIndex; i++)
		if(isEqual(pr.m_Vertex[i], v))
			{found = i; break;}

	if(found < 0)
	{
		pr.m_Vertex[writeIndex] = v;
		writeIndex++;
		return writeIndex - 1;
	}

	return (unsigned int)found;
}

bool CEditGraphObj::isEqual(const CVertex &v1, const CVertex &v2)
{
	return
		(v1.pos - v2.pos).abs() < 0.001 &&
		(v1.nor - v2.nor).abs() < 0.001 &&
		(v1.col - v2.col).abs() < 0.001 &&
		(v1.tex - v2.tex).abs() < 0.0001 &&
		fabs(v1.opacity - v2.opacity) < 0.01 &&
		fabs(v1.reflectance - v2.reflectance) < 0.01;
}

void CEditGraphObj::render(const CString &visibleLODs)
{
	//delete existing list
	if(isRendered)
	{
		printf("Deleting old model...\n");
		glDeleteLists(m_ObjList, 1);
		glDeleteLists(m_ObjListRef, 1);
	}

	printf("Generating model...\n");

	//Normal model
	m_ObjList = glGenLists(1);
	glNewList(m_ObjList, GL_COMPILE);

	for(unsigned int i=0; i<m_Primitives.size(); i++)
	{
		CPrimitive &pr = m_Primitives[i];

		CString LODs = pr.m_LODs;
		bool doit = false;
		for(unsigned int j=0; j < LODs.length(); j++)
			if(visibleLODs.inStr( (LODs[j]) ) >= 0)
				{doit = true; break;}
		if(!doit) continue;

		//Standard colors
		m_OpacityState = pr.m_Opacity;
		m_ColorState = pr.m_ModulationColor;

		//Apply texture
		if(pr.m_Texture < 0)
		{ //no texture
			glDisable(GL_TEXTURE_2D); 
		}
		else if(m_MatArray[pr.m_Texture]->getSizeX(1) <= 4 || m_MatArray[pr.m_Texture]->getSizeY(1) <= 4)
		{ //too small texture
			glDisable(GL_TEXTURE_2D);
			m_ColorState.x *= pr.m_ReplacementColor.x;
			m_ColorState.y *= pr.m_ReplacementColor.y;
			m_ColorState.z *= pr.m_ReplacementColor.z;
		}
		else
		{ //a good texture
			glEnable(GL_TEXTURE_2D);
			m_MatArray[pr.m_Texture]->draw(1);
		}

		//Set the color
		setMaterialColor();

		glBegin(GL_TRIANGLES);
		for(unsigned int k=0; k<pr.m_Index.size(); k++)
		{
			unsigned int j = pr.m_Index[k];
			CVertex &vt = pr.m_Vertex[j];
			CVector &nor = vt.nor;
			glNormal3f(nor.x, nor.y, nor.z);
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

		if(pr.m_Type == CPrimitive::VertexArray)
		{
			m_OpacityState = pr.m_Reflectance;
			setMaterialColor();

			glBegin(GL_TRIANGLES);
			for(unsigned int k=0; k<pr.m_Index.size(); k++)
			{
				unsigned int j = pr.m_Index[k];
				CVertex &vt = pr.m_Vertex[j];
				CVector &nor = vt.nor;
				glNormal3f(nor.x, nor.y, nor.z);
				CVector &pos = vt.pos;
				glVertex3f(pos.x, pos.y, pos.z);
			}
			glEnd();
		}
		else
		{
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
	}

	glEndList();

	
	isRendered = true;
}

void CEditGraphObj::draw() const
{
	glCallList(m_ObjList);
}

void CEditGraphObj::drawRef() const
{
	glCallList(m_ObjListRef);
}

void CEditGraphObj::setMaterialColor()
{
	float kleur[] = {m_ColorState.x, m_ColorState.y, m_ColorState.z, m_OpacityState};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, kleur);
}
