/***************************************************************************
                          loadlwo.cpp  -  LWO file loader
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

#include <cstdio>
#include "lw.h"

#include "datafile.h"

#include "loadlwo.h"

bool loadLWO(const CString &filename, CEditGraphObj &obj)
{
	CDataFile f(filename);
	CString fn2 = f.useExtern();

	if(!lw_is_lwobject(fn2.c_str())) return false;

	obj.clear();

	lwObject *lwo = lw_object_read(fn2.c_str());
	if(lwo == NULL) return false;

	printf("Loaded all data from LWO file\n");

	//first, create a primitive for every material
	for(int i=0; i < lwo->material_cnt; i++)
	{
		obj.m_Primitives.push_back(CPrimitive());
		CPrimitive &pr = obj.m_Primitives.back();

		pr.m_Name = lwo->material[i].name;
		pr.m_Type = CPrimitive::VertexArray;
		pr.m_Material.LODs = "1234c";
		pr.m_Material.emissivity = 0.0;
		pr.m_Material.opacity = 1.0;
		pr.m_Material.reflectance = 0.0;
		pr.m_Material.modulationColor = CVector(
			lwo->material[i].r,
			lwo->material[i].g,
			lwo->material[i].b
			);
		pr.m_Material.texture = -1;
		pr.m_Material.replacementColor = pr.m_Material.modulationColor;

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

		CPrimitive &thePrimitive = obj.m_Primitives[face->material];

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
