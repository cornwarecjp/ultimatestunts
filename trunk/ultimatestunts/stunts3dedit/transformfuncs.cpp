/***************************************************************************
                          transformfuncs.cpp  -  description
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

void rotateFunc()
{
	bool scene = getInput("Do you want to rotate the entire scene? ") == "y";

	CMatrix m;
	if(getInput("Do you want to rotate around the x-axis (y/n)? ")=="y")
	{
		float angle = getInput("Enter the angle in degrees: ").toFloat() * (3.1415926536/180.0);
		m.rotX(angle);
	}
	else if(getInput("Do you want to rotate around the y-axis (y/n)? ")=="y")
	{
		float angle = getInput("Enter the angle in degrees: ").toFloat() * (3.1415926536/180.0);
		m.rotY(angle);
	}
	else if(getInput("Do you want to rotate around the z-axis (y/n)? ")=="y")
	{
		float angle = getInput("Enter the angle in degrees: ").toFloat() * (3.1415926536/180.0);
		m.rotZ(angle);
	}
	else
	{
		CVector x = getInput("Enter the new position of the x-axis: ").toVector();
		CVector y = getInput("Enter the new position of the y-axis: ").toVector();
		CVector z = getInput("Enter the new position of the z-axis: ").toVector();

		m.setElement(0,0, x.x); m.setElement(0,1, y.x); m.setElement(0,2, z.x);
		m.setElement(1,0, x.y); m.setElement(1,1, y.y); m.setElement(1,2, z.y);
		m.setElement(2,0, x.z); m.setElement(2,1, y.z); m.setElement(2,2, z.z);
	}

	if(scene)
	{
		for(unsigned int i=0; i<graphobj->m_Primitives.size(); i++)
		{
			CPrimitive &pr = graphobj->m_Primitives[i];
			for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
			{
				CVertex &vt = pr.m_Vertex[j];
				vt.pos *= m;
				vt.nor *= m;
				vt.nor.normalise();
			}
		}
	}
	else
	{
			CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
			for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
			{
				CVertex &vt = pr.m_Vertex[j];
				vt.pos *= m;
				vt.nor *= m;
				vt.nor.normalise();
			}
	}

	graphobj->render(VisibleLODs);
}

void translateFunc()
{
	bool scene = getInput("Do you want to translate the entire scene? ") == "y";
	CVector v = getInput("Enter translation vector: ").toVector();

	if(scene)
	{
		for(unsigned int i=0; i<graphobj->m_Primitives.size(); i++)
		{
			CPrimitive &pr = graphobj->m_Primitives[i];
			for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
				pr.m_Vertex[j].pos += v;
		}
	}
	else
	{
		CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
		for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
			pr.m_Vertex[j].pos += v;
	}

	graphobj->render(VisibleLODs);
}

void rotateTextureFunc()
{
	CVector x = getInput("Enter the new position of the x-axis: ").toVector();
	CVector y = getInput("Enter the new position of the y-axis: ").toVector();

	CMatrix m;
	m.setElement(0,0, x.x); m.setElement(0,1, y.x);
	m.setElement(1,0, x.y); m.setElement(1,1, y.y);

	CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
	for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
		pr.m_Vertex[j].tex *= m;

	graphobj->render(VisibleLODs);
}

void translateTextureFunc()
{
	CVector v = getInput("Enter translation vector: ").toVector();

	CPrimitive &pr = graphobj->m_Primitives[curr_primitive];
	for(unsigned int j=0; j<pr.m_Vertex.size(); j++)
		pr.m_Vertex[j].tex += v;

	graphobj->render(VisibleLODs);
}

