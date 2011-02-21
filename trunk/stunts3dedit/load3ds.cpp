/***************************************************************************
                          load3ds.cpp  -  3DS file loader
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

#include "3ds.h"

#include "datafile.h"

#include "load3ds.h"

bool load3DS(const CString &filename, CEditGraphObj &obj)
{
	CDataFile f(filename);
	CString fn2 = f.useExtern();

	obj.clear();

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
		pr.m_Material.LODs = "1234c";
		pr.m_Material.emissivity = 0.0;
		pr.m_Material.opacity = 1.0;
		pr.m_Material.reflectance = 0.0;
		pr.m_Material.modulationColor = CVector(1,1,1);
		pr.m_Material.replacementColor = CVector(1,1,1);

		// Check to see if this object has a texture map, if so bind the texture to it.
		if(pObject->bHasTexture)
		{
			pr.m_Material.texture = -1;
		}
		else
		{
			//default to no texture even if there is a texture
			pr.m_Material.texture = -1;
			//pr.m_Material.texture = pObject->materialID;
		}

		//Position and orientation matrix:
		CVector ObjPos(pObject->objectCenter.x, pObject->objectCenter.y, pObject->objectCenter.z);
		CMatrix ObjOri;
		ObjOri.setElement(0, 0, pObject->Xaxis.x);
		ObjOri.setElement(0, 1, pObject->Xaxis.y);
		ObjOri.setElement(0, 2, pObject->Xaxis.z);
		ObjOri.setElement(1, 0, pObject->Yaxis.x);
		ObjOri.setElement(1, 1, pObject->Yaxis.y);
		ObjOri.setElement(1, 2, pObject->Yaxis.z);
		ObjOri.setElement(2, 0, pObject->Zaxis.x);
		ObjOri.setElement(2, 1, pObject->Zaxis.y);
		ObjOri.setElement(2, 2, pObject->Zaxis.z);

		//add the vertex array
		for(int j=0; j < pObject->numOfVerts; j++)
		{
			CVertex vt;

			vt.nor = CVector(pObject->pNormals[j].x, pObject->pNormals[j].y, pObject->pNormals[j].z);
			vt.pos = CVector(pObject->pVerts[j].x, pObject->pVerts[j].y, pObject->pVerts[j].z);

			//To absolute coordinates
			vt.pos = ObjOri * (vt.pos - ObjPos);

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
					pr.m_Material.modulationColor.x = (float)(pColor[0]) / 255.0;
					pr.m_Material.modulationColor.y = (float)(pColor[1]) / 255.0;
					pr.m_Material.modulationColor.z = (float)(pColor[2]) / 255.0;
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

		obj.m_Primitives.push_back(pr);
	}

	return true;
}
