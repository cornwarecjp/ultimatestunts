/***************************************************************************
                          editfuncs.cpp  -  edit functions for stunts3dedit
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

void primitiveFunc()
{
	for(unsigned int i=0; i<graphobj->m_Primitives.size(); i++)
		printf("%d: %s\n", i, graphobj->m_Primitives[i].m_Name.c_str());
	curr_primitive = getInput("Select primitive: ").toInt();
	curr_vertex = 0;
}

void vertexFunc()
{
	CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
	unsigned int nv = pr.m_Vertex.size();
	printf("%s has %d vertices:\n", pr.m_Name.c_str(), nv);
	for(unsigned int i=0; i<nv; i++)
		printf("\t%d: %f,%f,%f\n", i,
			pr.m_Vertex[i].pos.x,
			pr.m_Vertex[i].pos.y,
			pr.m_Vertex[i].pos.z);
	curr_vertex = getInput("Select vertex: ").toInt();
}

void newFunc()
{
	if(getInput("Do you want to add a new vertex (y/n)? ") == "y")
	{
		CVertex vt;
		vt.pos = getInput("Give position: ").toVector();
		vt.nor = getInput("Give normal: ").toVector();
		vt.tex = getInput("Give texcoord: ").toVector();
		graphobj->m_Primitives[curr_primitive].m_Vertex.push_back(vt);
	}
	else if(graphobj->m_Primitives[curr_primitive].m_Type == CPrimitive::VertexArray &&
		getInput("Do you want to add a new triangle (y/n)? ") == "y")
	{
		CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
		for(unsigned int i=0; i<pr.m_Vertex.size(); i++)
			printf("\t%d: %f,%f,%f\n", i,
				pr.m_Vertex[i].pos.x,
				pr.m_Vertex[i].pos.y,
				pr.m_Vertex[i].pos.z);
		
		CVector indices = getInput("Enter the vertex indices: ").toVector();
		pr.m_Index.push_back((unsigned int)indices.x);
		pr.m_Index.push_back((unsigned int)indices.y);
		pr.m_Index.push_back((unsigned int)indices.z);
	}
	else if(getInput("Do you want to add a new primitive (y/n)? ") == "y")
	{
		CPrimitive p;
		p.m_Type = CPrimitive::VertexArray;
		p.m_Name = getInput("Enter the name: ");
		p.m_Texture = getInput("Which texture should be attached? ").toInt();
		p.m_LODs = getInput("In which LODs should it be visible? ");
		p.m_ModulationColor = getInput("Modulation color: ").toVector();
		p.m_ReplacementColor = getInput("Texture replacement color: ").toVector();
		p.m_Opacity = getInput("Opacity: ").toFloat();
		p.m_Reflectance = getInput("Reflectance: ").toFloat();
		p.m_Emissivity = getInput("Emissivity: ").toFloat();
		p.m_StaticFriction = getInput("Static friction coefficient: ").toFloat();
		p.m_DynamicFriction = getInput("Dynamic friction coefficient: ").toFloat();
		graphobj->m_Primitives.push_back(p);
		curr_primitive = graphobj->m_Primitives.size()-1;
	}

	graphobj->render(VisibleLODs);
}

void changeFunc()
{
	if(getInput("Do you want to change a vertex (y/n)? ") == "y")
	{
		CVertex &vt = graphobj->m_Primitives[curr_primitive].m_Vertex[curr_vertex];
		printf("Pos: %f,%f,%f\n", vt.pos.x, vt.pos.y, vt.pos.z);
		printf("Nor: %f,%f,%f\n", vt.nor.x, vt.nor.y, vt.nor.z);
		printf("Tex: %f,%f\n", vt.tex.x, vt.tex.y);

		CString answ = getInput("Give new position: ");
		if(answ != "-")
			vt.pos = answ.toVector();

		answ = getInput("Give new normal: ");
		if(answ != "-")
			vt.nor = answ.toVector();

		answ = getInput("Give new texcoord: ");
		if(answ != "-")
			vt.tex = answ.toVector();
	}
	else if(getInput("Do you want to change the indices of a triangle (y/n)? ") == "y")
	{
		CPrimitive &pr = graphobj->m_Primitives[curr_primitive];

		for(unsigned int t=0; t < pr.m_Index.size()/3; t++)
			printf("Triangle %d: %d  %d  %d\n",
				t,
				pr.m_Index[3*t],
				pr.m_Index[3*t+1],
				pr.m_Index[3*t+2]);

		unsigned int t = (unsigned int)getInput("Select a triangle: ").toInt();

		printf("Current indices are: %d  %d  %d\n",
			pr.m_Index[3*t],
			pr.m_Index[3*t+1],
			pr.m_Index[3*t+2]);

		CString answ = getInput("Give new indices (\"-\" = no change): ");
		if(answ != "-")
		{
			CVector ind = answ.toVector();
			pr.m_Index[3*t] = (unsigned int)ind.x;
			pr.m_Index[3*t+1] = (unsigned int)ind.y;
			pr.m_Index[3*t+2] = (unsigned int)ind.z;
		}

	}

	graphobj->render(VisibleLODs);
}

void deleteFunc()
{
	if(getInput("Do you want to delete a primitive (y/n)? ") == "y")
	{
		for(unsigned int i=0; i<graphobj->m_Primitives.size(); i++)
			printf("%d: %s\n", i, graphobj->m_Primitives[i].m_Name.c_str());
		int p = getInput("Select primitive to delete (negative = cancel): ").toInt();
		if(p < 0) return;
		graphobj->m_Primitives.erase(graphobj->m_Primitives.begin() + (unsigned int)p);
		if(curr_primitive == p)
		{
			curr_primitive = 0;
			curr_vertex = 0;
		}
	}
	else if(getInput("Do you want to delete a vertex (y/n)? ") == "y")
	{
		CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
		unsigned int nv = pr.m_Vertex.size();
		printf("%s has %d vertices:\n", pr.m_Name.c_str(), nv);
		for(unsigned int i=0; i<nv; i++)
			printf("\t%d: %f,%f,%f\n", i,
				pr.m_Vertex[i].pos.x,
				pr.m_Vertex[i].pos.y,
				pr.m_Vertex[i].pos.z);
		int v = getInput("Select vertex to delete (negative = cancel): ").toInt();
		if(v < 0) return;
		if(curr_vertex == v) curr_vertex = 0;
		pr.m_Vertex.erase(pr.m_Vertex.begin() + (unsigned int)v);

		//also erase surrounding triangles
		for(unsigned int t=0; t < pr.m_Index.size()/3; t++)
		{
			int i1 = pr.m_Index[3*t], i2 = pr.m_Index[3*t+1], i3 = pr.m_Index[3*t+2];
			if(i1 == v || i2 == v || i3 == v)
			{
				printf("Also erasing triangle %d\n", t);
				pr.m_Index.erase(pr.m_Index.begin() + 3*t);
				pr.m_Index.erase(pr.m_Index.begin() + 3*t);
				pr.m_Index.erase(pr.m_Index.begin() + 3*t);
				t --; //re-do the same triangle nr again
			}
		}
		//then adapt the indices of the other triangles
		for(unsigned int i=0; i < pr.m_Index.size(); i++)
			if(pr.m_Index[i] > (unsigned int)v) pr.m_Index[i]--;
	}
	else if(getInput("Do you want to delete a triangle (y/n)? ") == "y")
	{
		printf("NYI\n");
	}

	graphobj->render(VisibleLODs);
}

void duplicatePrimitiveFunc()
{
	CPrimitive p = graphobj->m_Primitives[curr_primitive];
	graphobj->m_Primitives.push_back(p);
	curr_primitive =graphobj->m_Primitives.size()-1;
	printf("Primitive is duplicated\n");
	graphobj->render(VisibleLODs);
}
