/***************************************************************************
                          toolfuncs.cpp  -  Tool functions for stunts3dedit
                             -------------------
    begin                : vr okt 8 2004
    copyright            : (C) 2004 by CJP
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

void changePrimitiveFunc()
{
	CPrimitive &p = graphobj->m_Primitives[curr_primitive];
	printf("Name: %s\n", p.m_Name.c_str());
	printf("Type: ");
	switch(p.m_Type)
	{
	case CPrimitive::Triangles: printf("Triangles\n"); break;
	case CPrimitive::Quads: printf("Quads\n"); break;
	case CPrimitive::TriangleStrip: printf("Trianglestrip\n"); break;
	case CPrimitive::QuadStrip: printf("Quadstrip\n"); break;
	case CPrimitive::Polygon: printf("Polygon\n"); break;
	case CPrimitive::VertexArray: printf("Vertex array\n"); break;
	}
	printf("Texture: %d\n", p.m_Texture);
	printf("LODs: %s\n", p.m_LODs.c_str());
	printf("Modulation color: %s\n", CString(p.m_ModulationColor).c_str());
	printf("Texture replacement color: %s\n", CString(p.m_ReplacementColor).c_str());
	printf("Opacity: %.3f\n", p.m_Opacity);
	printf("Reflectance: %.3f\n", p.m_Reflectance);
	printf("Emissivity: %.3f\n", p.m_Emissivity);
	printf("Static friction coefficient: %.3f\n", p.m_StaticFriction);
	printf("Dynamic friction coefficient: %.3f\n", p.m_DynamicFriction);
	printf("Entering \"-\" will leave a property unchanged\n");

	CString answ = getInput("Enter new name: ");
	if(answ != "-")
		p.m_Name = answ;

	answ = getInput("Which texture should be attached? ");
	if(answ != "-")
		p.m_Texture = answ.toInt();

	answ = getInput("In which LODs should it be visible? ");
	if(answ != "-")
		p.m_LODs = answ;

	answ = getInput("Modulation color: ");
	if(answ != "-")
		p.m_ModulationColor = answ.toVector();

	answ = getInput("Texture replacement color: ");
	if(answ != "-")
		p.m_ReplacementColor = answ.toVector();

	answ = getInput("Opacity: ");
	if(answ != "-")
		p.m_Opacity = answ.toFloat();

	answ = getInput("Reflectance: ");
	if(answ != "-")
		p.m_Reflectance = answ.toFloat();

	answ = getInput("Emissivity: ");
	if(answ != "-")
		p.m_Emissivity = answ.toFloat();

	answ = getInput("Static friction coefficient: ");
	if(answ != "-")
		p.m_StaticFriction = answ.toFloat();

	answ = getInput("Dynamic friction coefficient: ");
	if(answ != "-")
		p.m_DynamicFriction = answ.toFloat();

	graphobj->render(VisibleLODs);
}

void scaleFunc()
{
	bool scene = getInput("Do you want to autoscale the entire scene (y/n)? ") == "y";
	bool cg = getInput("Do you want to scale around 1:the origin or 2:the CG? ") == "2";
	float maxs = getInput("Enter new size: ").toFloat();

	//determine center
	CVector center = CVector(0,0,0);
	if(cg)
		if(scene)
		{
			int num = 0;
			for(unsigned int i=0; i<graphobj->m_Primitives.size(); i++)
			{
				CPrimitive &pr = graphobj->m_Primitives[i];
				for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
				{
					center += pr.m_Vertex[j].pos;
					num++;
				}
			}
			center /= num;
		}
		else
		{
			int num = 0;
			CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
			for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
			{
				center += pr.m_Vertex[j].pos;
				num++;
			}
			center /= num;
		}

	//determine scalefactor
	float min = 1.0e10;
	float max = -min;
	if(scene)
	{
		for(unsigned int i=0; i<graphobj->m_Primitives.size(); i++)
		{
			CPrimitive &pr = graphobj->m_Primitives[i];
			for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
			{
				CVector p = pr.m_Vertex[j].pos - center;
				if(p.x > max) max = p.x;
				if(p.y > max) max = p.y;
				if(p.z > max) max = p.z;
				if(p.x < min) min = p.x;
				if(p.y < min) min = p.y;
				if(p.z < min) min = p.z;
			}
		}
	}
	else
	{
		CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
		for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
		{
			CVector p = pr.m_Vertex[j].pos - center;
			if(p.x > max) max = p.x;
			if(p.y > max) max = p.y;
			if(p.z > max) max = p.z;
			if(p.x < min) min = p.x;
			if(p.y < min) min = p.y;
			if(p.z < min) min = p.z;
		}
	}

	float sf = maxs / (max-min);
	printf("Scaling with scalefactor %f\n", sf);

	//Apply scaling
	if(scene)
	{
		for(unsigned int i=0; i<graphobj->m_Primitives.size(); i++)
		{
			CPrimitive &pr = graphobj->m_Primitives[i];
			for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
				pr.m_Vertex[j].pos = center + (pr.m_Vertex[j].pos-center)*sf;
		}
	}
	else
	{
			CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
			for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
				pr.m_Vertex[j].pos = center + (pr.m_Vertex[j].pos-center)*sf;
	}

	graphobj->render(VisibleLODs);
}

void mirrorFunc()
{
	bool normals = getInput("Do you want to mirror the normals? ") == "y";
	bool vertices = getInput("Do you want to mirror the vertices? ") == "y";
	CPrimitive &pr = graphobj->m_Primitives[curr_primitive];

	if(normals)
	{
		for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
		{
			CVertex &vt = pr.m_Vertex[j];
			if(normals)
				vt.nor = -vt.nor;
		}
	}

	if(vertices)
	{
		unsigned int size =pr.m_Vertex.size();
		for(unsigned int j=0; j<size/2; j++)
		{
			CVertex vt = pr.m_Vertex[j];  //swapping the vertices; putting in reverse order
			pr.m_Vertex[j] = pr.m_Vertex[size-j-1];
			pr.m_Vertex[size-j-1] = vt;
		}
	}

	graphobj->render(VisibleLODs);
}

void setCollisionFunc()
{
	if(getInput("Do you want to apply the collision tool on the current primitive? ") != "y")
		return;

	CPrimitive &pr = graphobj->m_Primitives[curr_primitive];

	if(pr.m_Type != GL_QUADS)
	{
		printf("This function only works with quads!\n");
		return;
	}

	if(pr.m_LODs != "c")
	{
		printf("This primitive is not collision-LOD only (LOD must be \"c\")\n");
		return;
	}

	unsigned int numPlanes = pr.m_Vertex.size() / 4; //rounded to lower value if necessary
	for(unsigned int plane=0; plane < numPlanes; plane++)
	{
		//determine the plane normal and d

		//the first point
		CVector p1 = pr.m_Vertex[4*plane].pos;

		//the second point: as far away from 1 as possible
		unsigned int index_2nd = 1;
		CVector p2 = pr.m_Vertex[4*plane + index_2nd].pos;
		float dist2 = (p2-p1).abs2();
		for(unsigned int j=2; j<4; j++)
		{
			float d2 = (pr.m_Vertex[4*plane + j].pos - p1).abs2();
			if(d2 > dist2)
				{p2 = pr.m_Vertex[4*plane + j].pos; dist2 = d2; index_2nd = j;}
		}

		//the third point: try to maximise cross product
		CVector crosspr;
		float crosspr_abs2 = 0.0;
		bool reverse = false; //2 and 3 in reverse order
		CVector p1p2 = p2 - p1;
		for(unsigned int j=1; j<4; j++)
		{
			const CVector &p3 = pr.m_Vertex[4*plane + j].pos;
			CVector cp = (p3-p1).crossProduct(p1p2);
			float abs2 = cp.abs2();

			if(abs2 > crosspr_abs2)
			{
				crosspr = cp;
				crosspr_abs2 = abs2;
				reverse = (j<index_2nd);
			}
		}

		if(crosspr_abs2 < 0.00001) //too small
		{
			printf("There is a quad in the primitive which is more line- or point-like\n");
			return;
		}

		CVector nor;
		if(reverse) //2 and 3 in reverse order
			nor = crosspr.normal();
		else
			nor = -crosspr.normal();

		float dplane = p1.dotProduct(nor);

		//determine the max object d
		float dobj = -1000.0;
		for(unsigned int prim=0; prim < graphobj->m_Primitives.size(); prim++)
		{
			if((int)prim == curr_primitive) continue;

			for(unsigned int pt=0; pt < graphobj->m_Primitives[prim].m_Vertex.size(); pt++)
			{
				float d = graphobj->m_Primitives[prim].m_Vertex[pt].pos.dotProduct(nor);
				if(d > dobj) dobj = d;
			}
		}

		//translate plane vertices
		CVector dpos = (dobj - dplane) * nor;
		for(unsigned int i=0; i < 4; i++)
			pr.m_Vertex[4*plane + i].pos += dpos;
	}

	graphobj->render(VisibleLODs);
}
