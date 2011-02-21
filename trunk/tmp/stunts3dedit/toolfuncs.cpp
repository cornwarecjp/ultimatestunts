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
	printf("Name                     : %s\n", p.m_Name.c_str());
	printf("Texture                  : %d\n", p.m_Material.texture);
	printf("LODs                     : %s\n", p.m_Material.LODs.c_str());
	printf("Modulation color         : %s\n", CString(p.m_Material.modulationColor).c_str());
	printf("Texture replacement color: %s\n", CString(p.m_Material.replacementColor).c_str());
	printf("Opacity                  : %.3f\n", p.m_Material.opacity);
	printf("Reflectance              : %.3f\n", p.m_Material.reflectance);
	//printf("Emissivity               : %.3f\n", p.m_Material.emissivity);
	if(p.m_Animation.rotationEnabled)
	{
		printf("Rotation animation enabled:\n");
		printf("  origin  : %s\n", CString(p.m_Animation.rotationOrigin  ).c_str());
		printf("  velocity: %s\n", CString(p.m_Animation.rotationVelocity).c_str());
	}
	else
	{
		printf("Rotation animation disabled\n");
	}
	printf("\n");
	printf("Entering \"-\" will leave a property unchanged\n");

	CString answ = getInput("Enter new name: ");
	if(answ != "-")
		p.m_Name = answ;

	answ = getInput("Which texture should be attached? ");
	if(answ != "-")
		p.m_Material.texture = answ.toInt();

	answ = getInput("In which LODs should it be visible? ");
	if(answ != "-")
		p.m_Material.LODs = answ;

	answ = getInput("Modulation color: ");
	if(answ != "-")
		p.m_Material.modulationColor = answ.toVector();

	answ = getInput("Texture replacement color: ");
	if(answ != "-")
		p.m_Material.replacementColor = answ.toVector();

	answ = getInput("Opacity: ");
	if(answ != "-")
		p.m_Material.opacity = answ.toFloat();

	answ = getInput("Reflectance: ");
	if(answ != "-")
		p.m_Material.reflectance = answ.toFloat();

	//Not yet used:
	/*
	answ = getInput("Emissivity: ");
	if(answ != "-")
		p.m_Material.emissivity = answ.toFloat();
	*/

	answ = getInput("Enable rotation animation (y/n): ");
	if(answ != "-")
		p.m_Animation.rotationEnabled = answ == "y";

	if(p.m_Animation.rotationEnabled)
	{
		answ = getInput("Rotation animation origin: ");
		if(answ != "-")
			p.m_Animation.rotationOrigin = answ.toVector();

		answ = getInput("Rotation animation velocity: ");
		if(answ != "-")
			p.m_Animation.rotationVelocity = answ.toVector();
	}

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
	{
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
	printf("Scaling with scalefactor %f / (%f-%f) = %f\n", maxs, max, min, sf);

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

	if(pr.m_Material.LODs != "c")
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
	else if(getInput("Generate texture coordinates for this primitive (y/n)? ") == "y")
	{
		if(getInput("Use plane projection (y/n)? ") == "y")
		{
			CVector normal = getInput("Enter plane normal vector: ").toVector();
			CMatrix Rmat; Rmat.targetZ(normal, true);
			for(unsigned int v=0; v<pr.m_Vertex.size(); v++)
			{
				CVector p = Rmat * pr.m_Vertex[v].pos;
				p.z = 0.0;
				pr.m_Vertex[v].tex = p;
			}
		}
		else if(getInput("Use cylinder projection (y/n)? ") == "y")
		{
			CVector axispos = getInput("Enter cylinder axis position: ").toVector();
			CVector normal = getInput("Enter cylinder axis direction: ").toVector();
			CMatrix Rmat; Rmat.targetZ(normal, true);
			for(unsigned int v=0; v<pr.m_Vertex.size(); v++)
			{
				CVector p = Rmat * (pr.m_Vertex[v].pos - axispos);
				pr.m_Vertex[v].tex = CVector(atan2f(p.y,p.x)/(2*M_PI), p.z, 0.0);
			}
		}
	}
	else if(getInput("Generate texture replacement colors for all primitives (y/n)? ") == "y")
	{
		//for every primitive
		for(unsigned int p=0; p<graphobj->m_Primitives.size(); p++)
		{
			CPrimitive &thePrimitive = graphobj->m_Primitives[p];
			if(thePrimitive.m_Material.texture >= 0)
			{
				CLODTexture *texture = graphobj->m_MatArray[thePrimitive.m_Material.texture];
				thePrimitive.m_Material.replacementColor = texture->getColor();
			}
			else
			{
				thePrimitive.m_Material.replacementColor = CVector(1,1,1);
			}
		}
	}

	graphobj->render(VisibleLODs);
}

void splitFunc()
{
	CString axis = getInput("Give axis (x, y or z):");
	float limit  = getInput("Give limit value:").toFloat();
	bool above   = getInput("Do you want to keep everything above this value? (y/n):") == "y";

	CVector normal;
	if(axis == "x")
		{normal = CVector(1,0,0);}
	else if(axis == "y")
		{normal = CVector(0,1,0);}
	else if(axis == "z")
		{normal = CVector(0,0,1);}
	else
		{return;}

	if(!above) normal = -normal;

	for(unsigned int p=0; p<graphobj->m_Primitives.size(); p++)
	{
		CPrimitive &pr = graphobj->m_Primitives[p];

		//First delete triangles
		int numPlanes = pr.m_Index.size() / 3; //rounded to lower value if necessary
		for(int plane=0; plane < numPlanes; plane++)
		{
			CVector p1 = pr.m_Vertex[pr.m_Index[3*plane]].pos;
			CVector p2 = pr.m_Vertex[pr.m_Index[3*plane+1]].pos;
			CVector p3 = pr.m_Vertex[pr.m_Index[3*plane+2]].pos;

			if(p1.dotProduct(normal) < limit ||
				p2.dotProduct(normal) < limit ||
				p3.dotProduct(normal) < limit)
			{
				//delete the triangle
				pr.m_Index.erase(pr.m_Index.begin()+3*plane);
				pr.m_Index.erase(pr.m_Index.begin()+3*plane);
				pr.m_Index.erase(pr.m_Index.begin()+3*plane);

				numPlanes--;
				plane--; //do the same index again
			}
		}

		//Then delete vertices
		for(int i=0; i < int(pr.m_Vertex.size()); i++)
			if(pr.m_Vertex[i].pos.dotProduct(normal) < limit)
			{
				pr.m_Vertex.erase(pr.m_Vertex.begin()+i);

				//update indices
				for(unsigned int j=0; j < pr.m_Index.size(); j++)
					if(pr.m_Index[j] > (unsigned int)i)
						pr.m_Index[j]--;

				i--; //do the same index again
			}
	}

	graphobj->render(VisibleLODs);
}

void orderFunc()
{
	printf("Swap the order of two primitives.\n");
	for(unsigned int i=0; i<graphobj->m_Primitives.size(); i++)
		printf("%d: %s\n", i, graphobj->m_Primitives[i].m_Name.c_str());
	int p1 = getInput("Select the first primitive (negative=cancel): ").toInt();
	if(p1<0) return;
	if(p1 >= (int)(graphobj->m_Primitives.size()))
	{
		printf("Selected primitive exceeds number of primitives\n");
		return;
	}

	int p2 = getInput("Select the second primitive (negative=cancel): ").toInt();
	if(p2<0) return;
	if(p2 >= (int)(graphobj->m_Primitives.size()))
	{
		printf("Selected primitive exceeds number of primitives\n");
		return;
	}

	if(p1==p2) return;

	CPrimitive tmp = graphobj->m_Primitives[p1];
	graphobj->m_Primitives[p1] = graphobj->m_Primitives[p2];
	graphobj->m_Primitives[p2] = tmp;

	if(curr_primitive == p1)
		{curr_primitive = p2;}
	else if(curr_primitive == p2)
		{curr_primitive = p1;}

	graphobj->render(VisibleLODs);
}

