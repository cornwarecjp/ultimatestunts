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

#include "load3ds.h"
#include "loadglt.h"
#include "loadlwo.h"
#include "loadobj.h"
#include "loadraw.h"
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

		pr2.m_Name = pr1.Name;
		pr2.m_Type = CPrimitive::VertexArray;

		pr2.m_Material.LODs = "";
		if(pr1.material.LODs & 1) pr2.m_Material.LODs += '1';
		if(pr1.material.LODs & 2) pr2.m_Material.LODs += '2';
		if(pr1.material.LODs & 4) pr2.m_Material.LODs += '3';
		if(pr1.material.LODs & 8) pr2.m_Material.LODs += '4';
		if(pr1.material.LODs & 16) pr2.m_Material.LODs += 'c';
		if(pr1.material.LODs & 32) pr2.m_Material.LODs += 's';
		pr2.m_Material.texture = pr1.material.Texture;
		pr2.m_Material.modulationColor = pr1.material.ModulationColor;
		pr2.m_Material.replacementColor = pr1.material.ReplacementColor;
		pr2.m_Material.opacity = pr1.material.Opacity;
		pr2.m_Material.reflectance = pr1.material.Reflectance;
		pr2.m_Material.emissivity = pr1.material.Emissivity;

		pr2.m_Animation.rotationEnabled = (pr1.animation.AnimationFlags & 0x1) != 0;
		pr2.m_Animation.textureEnabled = (pr1.animation.AnimationFlags & 0x2) != 0;
		pr2.m_Animation.rotationOrigin = pr1.animation.rotationOrigin;
		pr2.m_Animation.rotationVelocity = pr1.animation.rotationVelocity;
		pr2.m_Animation.texturePeriod = pr1.animation.texturePeriod;
		pr2.m_Animation.textures = pr1.animation.textures;

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

bool CEditGraphObj::loadGLTFile(CString filename)
{
	return loadGLT(filename, *this);
}

bool CEditGraphObj::loadRAWFile(CString filename)
{
	return loadRAW(filename, *this);
}

bool CEditGraphObj::load3DSFile(CString filename)
{
	return load3DS(filename, *this);
}

bool CEditGraphObj::loadLWOFile(CString filename)
{
	return loadLWO(filename, *this);
}

bool CEditGraphObj::loadOBJFile(CString filename)
{
	return loadOBJ(filename, *this);
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
		thePrimitive.m_Material.LODs = lods;
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
	f.writel("Lod 1234c");
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
		if(!(pr.m_Material.LODs == LODs))
		{
			LODs = pr.m_Material.LODs;
			f.writel("Lod " + LODs);

			//rewrite the entire state (set the current state totally invalid)
			//TODO: more efficient
			texid = -2;
			state.col = state.nor = state.tex = CVector(2,2,2);
			state.opacity = state.reflectance = 2;
		}
		if(!(pr.m_Material.texture == texid))
		{
			texid = pr.m_Material.texture;
			if(texid < 0)
				f.writel("Notex");
			else
				f.writel(CString("Texture ") + texid);
		}

		f.writel("VertexArray");
		f.writel(CString("Color ") + pr.m_Material.modulationColor);
		f.writel(CString("Opacity ") + pr.m_Material.opacity);
		f.writel(CString("ReplacementColor ") + pr.m_Material.replacementColor);
		f.writel(CString("Reflectance ") + pr.m_Material.reflectance);
		f.writel(CString("Emissivity ") + pr.m_Material.emissivity);

		if(pr.m_Animation.rotationEnabled)
		{
			f.writel(CString("RotationAnimation ") +
				pr.m_Animation.rotationOrigin + ";" +
				pr.m_Animation.rotationVelocity);
		}

		if(pr.m_Animation.textureEnabled)
		{
			CString line = CString("TextureAnimation ");
			line += CString(pr.m_Animation.texturePeriod);
			line += ";";
			if(pr.m_Animation.textures.size() > 0)
			{
				line += CString(pr.m_Animation.textures[0]);
				if(pr.m_Animation.textures.size() > 1)
					for(unsigned int i=1; i < pr.m_Animation.textures.size(); i++)
						line += CString(",") + pr.m_Animation.textures[i];
			}
			f.writel(line);
		}
		
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

		pr2.Name = pr1.m_Name;

		//Animation
		pr2.animation.AnimationFlags = 0;
		if(pr1.m_Animation.rotationEnabled) pr2.animation.AnimationFlags |= 0x1;
		if(pr1.m_Animation.textureEnabled)  pr2.animation.AnimationFlags |= 0x2;
		pr2.animation.rotationOrigin   = pr1.m_Animation.rotationOrigin;
		pr2.animation.rotationVelocity = pr1.m_Animation.rotationVelocity;
		pr2.animation.texturePeriod = pr1.m_Animation.texturePeriod;
		pr2.animation.textures = pr1.m_Animation.textures;

		//Material
		pr2.material.LODs = 0;
		if(pr1.m_Material.LODs.inStr('1') >= 0) pr2.material.LODs += 1;
		if(pr1.m_Material.LODs.inStr('2') >= 0) pr2.material.LODs += 2;
		if(pr1.m_Material.LODs.inStr('3') >= 0) pr2.material.LODs += 4;
		if(pr1.m_Material.LODs.inStr('4') >= 0) pr2.material.LODs += 8;
		if(pr1.m_Material.LODs.inStr('c') >= 0) pr2.material.LODs += 16;
		if(pr1.m_Material.LODs.inStr('s') >= 0) pr2.material.LODs += 32;
		pr2.material.Emissivity = pr1.m_Material.emissivity;
		pr2.material.ModulationColor = pr1.m_Material.modulationColor;
		pr2.material.Opacity = pr1.m_Material.opacity;
		pr2.material.Reflectance = pr1.m_Material.reflectance;
		pr2.material.ReplacementColor = pr1.m_Material.replacementColor;
		pr2.material.Texture = pr1.m_Material.texture;

		//Vertices
		for(unsigned int v=0; v < pr1.m_Vertex.size(); v++)
		{
			CGLBFile::SVertex vt2;
			CVertex &vt1 = pr1.m_Vertex[v];

			vt2.pos = vt1.pos;
			vt2.nor = vt1.nor;
			vt2.tex = vt1.tex;

			pr2.vertex.push_back(vt2);
		}

		//Indices
		pr2.index = pr1.m_Index;
	}

	f.save(filename);
}

void CEditGraphObj::convertToVertexArrays()
{
	for(unsigned int p=0; p < m_Primitives.size(); p++)
	{
		CPrimitive &pr = m_Primitives[p];

		pr.m_Material.modulationColor = pr.m_Vertex[0].col;
		pr.m_Material.opacity = pr.m_Vertex[0].opacity;
		pr.m_Material.reflectance = pr.m_Vertex[0].reflectance;
		if(pr.m_Material.texture >= 0)
		{
			pr.m_Material.replacementColor = m_MatArray[pr.m_Material.texture]->getColor();
			pr.m_Material.replacementColor.x *= pr.m_Material.modulationColor.x;
			pr.m_Material.replacementColor.y *= pr.m_Material.modulationColor.y;
			pr.m_Material.replacementColor.z *= pr.m_Material.modulationColor.z;
		}
		else
			{pr.m_Material.replacementColor = pr.m_Material.modulationColor;}
		pr.m_Material.emissivity = 0;

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

		CString LODs = pr.m_Material.LODs;
		bool doit = false;
		for(unsigned int j=0; j < LODs.length(); j++)
			if(visibleLODs.inStr( (LODs[j]) ) >= 0)
				{doit = true; break;}
		if(!doit) continue;

		//Standard colors
		m_OpacityState = pr.m_Material.opacity;
		m_ColorState = pr.m_Material.modulationColor;

		//Apply texture
		if(pr.m_Material.texture < 0)
		{ //no texture
			glDisable(GL_TEXTURE_2D);
		}
		else if(
			m_MatArray[pr.m_Material.texture]->getSizeX(1) <= 4 ||
			m_MatArray[pr.m_Material.texture]->getSizeY(1) <= 4)
		{ //too small texture
			glDisable(GL_TEXTURE_2D);
			m_ColorState.x *= pr.m_Material.replacementColor.x;
			m_ColorState.y *= pr.m_Material.replacementColor.y;
			m_ColorState.z *= pr.m_Material.replacementColor.z;
		}
		else
		{ //a good texture
			glEnable(GL_TEXTURE_2D);
			m_MatArray[pr.m_Material.texture]->draw(1);
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

		//Rotation axis
		if(pr.m_Animation.rotationEnabled)
		{
			CVector p1 = pr.m_Animation.rotationOrigin;
			CVector p2 = p1 + pr.m_Animation.rotationVelocity;

			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);
			glColor4f(0,1,1,1);

			glBegin(GL_LINES);
			glVertex3f(p1.x, p1.y, p1.z);
			glVertex3f(p2.x, p2.y, p2.z);
			glEnd();

			glColor4f(1,1,1,1);
			glEnable(GL_LIGHTING);
		}
	}

	glEndList();

	//Reflection model
	m_ObjListRef = glGenLists(1);
	glNewList(m_ObjListRef, GL_COMPILE);

	m_ColorState = CVector(1,1,1);
	for(unsigned int i=0; i<m_Primitives.size(); i++)
	{
		CPrimitive &pr = m_Primitives[i];
		if(pr.m_Material.LODs.inStr('1') < 0) continue; //reflection model = LOD 1

		if(pr.m_Type == CPrimitive::VertexArray)
		{
			m_OpacityState = pr.m_Material.reflectance;
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
