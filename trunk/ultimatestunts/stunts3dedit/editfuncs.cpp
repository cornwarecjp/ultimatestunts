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
		vt.col = getInput("Give color: ").toVector();
		vt.opacity = getInput("Give opacity: ").toFloat();
		vt.reflectance = getInput("Give reflectance: ").toFloat();
		vt.tex = getInput("Give texcoord: ").toVector();
		graphobj->m_Primitives[curr_primitive].m_Vertex.push_back(vt);
	}
	else if(getInput("Do you want to add a new primitive (y/n)? ") == "y")
	{
		CPrimitive p;
		p.m_Name = getInput("Enter the name: ");
		printf("Choose the type:\n"
			"1: Triangles\n"
			"2: Quads\n"
			"3: Trianglestrip\n"
			"4: Quadstrip\n"
			"5: Polygon\n"
		);
		switch(getInput(": ").toInt())
		{
		case 1: p.m_Type = GL_TRIANGLES; break;
		case 2: p.m_Type = GL_QUADS; break;
		case 3: p.m_Type = GL_TRIANGLE_STRIP; break;
		case 4: p.m_Type = GL_QUAD_STRIP; break;
		case 5: p.m_Type = GL_POLYGON; break;
		}
		p.m_Texture = getInput("Which texture should be attached? ").toInt();
		p.m_LODs = getInput("In which LODs should it be visible? ");
		graphobj->m_Primitives.push_back(p);
		curr_primitive = graphobj->m_Primitives.size()-1;
	}

	graphobj->render(VisibleLODs);
}

void changeFunc()
{
	CVertex &vt = graphobj->m_Primitives[curr_primitive].m_Vertex[curr_vertex];
	printf("Pos: %f,%f,%f\n", vt.pos.x, vt.pos.y, vt.pos.z);
	printf("Nor: %f,%f,%f\n", vt.nor.x, vt.nor.y, vt.nor.z);
	printf("Col: %f,%f,%f\n", vt.col.x, vt.col.y, vt.col.z);
	printf("Opacity: %f\n", vt.opacity);
	printf("Reflectance: %f\n", vt.reflectance);
	printf("Tex: %f,%f\n", vt.tex.x, vt.tex.y);

	CString answ = getInput("Give new position: ");
	if(answ != "-")
		vt.pos = answ.toVector();

	answ = getInput("Give new normal: ");
	if(answ != "-")
		vt.nor = answ.toVector();

	answ = getInput("Give new color: ");
	if(answ != "-")
		vt.col = answ.toVector();

	answ = getInput("Give new opacity: ");
	if(answ != "-")
		vt.opacity = answ.toFloat();

	answ = getInput("Give new reflectance: ");
	if(answ != "-")
		vt.reflectance = answ.toFloat();

	answ = getInput("Give new texcoord: ");
	if(answ != "-")
		vt.tex = answ.toVector();

	graphobj->render(VisibleLODs);
}

void duplicatePrimitiveFunc()
{
	CPrimitive p = graphobj->m_Primitives[curr_primitive];
	graphobj->m_Primitives.push_back(p);
	curr_primitive =graphobj->m_Primitives.size()-1;
	graphobj->render(VisibleLODs);
}
