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

	//Not yet used:
	/*
	answ = getInput("Emissivity: ");
	if(answ != "-")
		p.m_Emissivity = answ.toFloat();

	answ = getInput("Static friction coefficient: ");
	if(answ != "-")
		p.m_StaticFriction = answ.toFloat();

	answ = getInput("Dynamic friction coefficient: ");
	if(answ != "-")
		p.m_DynamicFriction = answ.toFloat();
	*/

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
		unsigned int size =pr.m_Index.size();
		for(unsigned int j=0; j<size; j+=3)
		{
			unsigned int t = pr.m_Index[j];  //swapping the vertices; putting in reverse order
			pr.m_Index[j] = pr.m_Index[j+2];
			pr.m_Index[j+2] = t;
		}
	}

	graphobj->render(VisibleLODs);
}

void setCollisionFunc()
{
	if(getInput("Do you want to apply the collision tool on the current primitive? ") != "y")
		return;

	CPrimitive &pr = graphobj->m_Primitives[curr_primitive];

	if(pr.m_LODs != "c")
	{
		printf("This primitive is not collision-LOD only (LOD must be \"c\")\n");
		return;
	}

	unsigned int numPlanes = pr.m_Index.size() / 3; //rounded to lower value if necessary
	for(unsigned int plane=0; plane < numPlanes; plane++)
	{
		//determine the plane normal and d

		//the first point
		CVector &p1 = pr.m_Vertex[pr.m_Index[3*plane]].pos;
		CVector &p2 = pr.m_Vertex[pr.m_Index[3*plane+1]].pos;
		CVector &p3 = pr.m_Vertex[pr.m_Index[3*plane+2]].pos;

		//the cross product
		CVector crosspr = (p3-p1).crossProduct(p1 - p2);

		if(crosspr.abs2() < 0.00001) //too small
		{
			printf("There is a triangle in the primitive which is more line- or point-like\n");
			return;
		}

		CVector nor = crosspr.normal();
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
		p1 += dpos;
		p2 += dpos;
		p3 += dpos;
	}

	graphobj->render(VisibleLODs);
}

void clampFunc()
{
	float in = getInput("Clamp in value:").toFloat();
	float out = getInput("Clamp out value:").toFloat();

	for(unsigned int i=0; i<graphobj->m_Primitives.size(); i++)
	{
		CPrimitive &pr = graphobj->m_Primitives[i];
		for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
		{
			CVector p = pr.m_Vertex[j].pos;

			if(fabs(p.x) > in - 0.01 && fabs(p.x) < in + 0.01)
				p.x = copysign(out, p.x);
			if(fabs(p.y) > in - 0.01 && fabs(p.y) < in + 0.01)
				p.y = copysign(out, p.y);
			if(fabs(p.z) > in - 0.01 && fabs(p.z) < in + 0.01)
				p.z = copysign(out, p.z);

			pr.m_Vertex[j].pos = p;
		}
	}
	
	graphobj->render(VisibleLODs);
}

void generateFunc()
{
	CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
	if(getInput("Generate normal vectors for this primitive (y/n)? ") == "y")
	{
		//for every vertex
		for(unsigned int v=0; v<pr.m_Vertex.size(); v++)
		{
			vector<CVector> foundNormals;

			//for every triangle
			for(unsigned int i=0; i<pr.m_Index.size(); i+= 3)
			{
				unsigned int i1 = pr.m_Index[i], //the vertex indices
					i2 = pr.m_Index[i+1],
					i3 = pr.m_Index[i+2];

				if(i1 == v || i2 == v || i3 == v) //the vertex is in this triangle
				{
					CVector p1 = pr.m_Vertex[i1].pos, //the vertex positions
						p2 = pr.m_Vertex[i2].pos,
						p3 = pr.m_Vertex[i3].pos;

					CVector normal = (p2-p1).crossProduct(p3-p1).normal();
					foundNormals.push_back(normal);
				}
			}

			if(foundNormals.size() > 0) //else: keep it unchanged
			{
				CVector normal;
				for(unsigned int i=0; i < foundNormals.size(); i++)
					normal += foundNormals[i];

				normal.normalise();
				pr.m_Vertex[v].nor = normal;
			}
		}
	}

	if(getInput("Generate texture replacement colors for all primitives (y/n)? ") == "y")
	{
		//for every primitive
		for(unsigned int p=0; p<graphobj->m_Primitives.size(); p++)
		{
			CPrimitive &thePrimitive = graphobj->m_Primitives[p];
			if(thePrimitive.m_Texture >= 0)
			{
				CLODTexture *texture = graphobj->m_MatArray[thePrimitive.m_Texture];
				thePrimitive.m_ReplacementColor = texture->getColor();
			}
			else
			{
				thePrimitive.m_ReplacementColor = CVector(1,1,1);
			}
		}
	}

	graphobj->render(VisibleLODs);
}
