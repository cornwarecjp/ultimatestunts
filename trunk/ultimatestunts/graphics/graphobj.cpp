/***************************************************************************
                          graphobj.cpp  -  Graphics geometry object
                             -------------------
    begin                : Tue Apr 23 2002
    copyright            : (C) 2002 by CJP
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
#include <cstdlib>
#include <cmath>
#include "pi.h"

#include <GL/gl.h>

#include "glextensions.h"
#include "vector.h"
#include "usmacros.h"
#include "lconfig.h"
//#include "graphicworld.h"
#include "datafile.h"
#include "glbfile.h"
#include "lodtexture.h"

#include "graphobj.h"

CGraphObj::CGraphObj(CDataManager *manager, eDataType type) : CDataObject(manager, type)
{
	m_CurrentTime = 0.0; //Initial value doesn't matter, as long as it isn't NaN, inf etc.
}

CGraphObj::~CGraphObj()
{
	unload();
}

bool CGraphObj::load(const CString &filename, const CParamList &list)
{
	CGLBFile f;
	if(!f.load(filename)) return false;

	CDataObject::load(filename, list);

	CString subset = m_ParamList.getValue("subset", "");
	vector<CDataObject *> matarray = m_DataManager->getSubset(CDataObject::eMaterial, subset);

	unsigned int waterTesselation = theMainConfig->getValue("animation", "watertesselation").toInt();

	//the primitives
	for(unsigned int p=0; p < f.m_Primitives.size(); p++)
	{
		CGLBFile::SPrimitive &pr1 = f.m_Primitives[p];

		m_Primitives.push_back(SPrimitive());
		SPrimitive &pr2 = m_Primitives.back();

		//Material
		pr2.LODs = pr1.material.LODs;
		pr2.opacity = pr1.material.Opacity;
		pr2.reflectance = pr1.material.Reflectance;
		pr2.emissivity = pr1.material.Emissivity;

		//Color and texture:
		//default:
		pr2.texture[0] = pr2.texture[1] = pr2.texture[2] = pr2.texture[3] = 0;
		pr2.color[0] = pr2.color[1] = pr2.color[2] = pr2.color[3] = pr1.material.ModulationColor;

		//Process the user-defined color
		if(pr1.material.Opacity < 0.01 && pr1.material.ModulationColor.z > 0.99)
		{
			printf("Changing to user-defined color\n");
			pr2.opacity = 1.0;
			pr2.color[0] = pr2.color[1] = pr2.color[2] = pr2.color[3] =
				list.getValue("color", "1,1,1").toVector();
		}

		//add texture
		if(pr1.material.Texture >= 0)
		{
			CLODTexture *tex = (CLODTexture *)matarray[pr1.material.Texture];

			for(unsigned int lod=0; lod < 4; lod++)
			{
				if(tex->getSizeX(lod+1) <=4 || tex->getSizeY(lod+1) <= 4)
				{
					//TODO: blend modulation & replacement color
					pr2.color[lod] = pr1.material.ReplacementColor;
				}
				else
				{
					pr2.texture[lod] = tex->getTextureID(lod+1);
				}
			}
			
		}

		//Animation
		pr2.animation.rotationEnabled  = (pr1.animation.AnimationFlags & 0x1) != 0;
		pr2.animation.rotationOrigin   = pr1.animation.rotationOrigin;
		pr2.animation.rotationVelocity = pr1.animation.rotationVelocity;

		//For water animation:
		if(waterTesselation != 0 && (pr1.material.LODs & 16) != 0 && (pr1.material.LODs & 32) != 0)
		{
			m_WaterLevel = pr1.vertex[0].pos.y;
			tesselateSquare(pr1, waterTesselation);
		}

		//add vertices
		pr2.numVertices = pr1.vertex.size();
		pr2.originalPos.resize(pr2.numVertices);
		pr2.originalNor.resize(pr2.numVertices);
		GLfloat *vertexptr = new GLfloat[8*pr1.vertex.size()];
		for(unsigned int v=0; v < pr1.vertex.size(); v++)
		{
			CGLBFile::SVertex &vt = pr1.vertex[v];

			pr2.originalPos[v] = vt.pos;
			pr2.originalNor[v] = vt.nor;

			GLfloat *offset = vertexptr + 8*v;

			//GL_T2F_N3F_V3F array format:
			*(offset  ) = vt.tex.x;
			*(offset+1) = vt.tex.y;
			
			*(offset+2) = vt.nor.x;
			*(offset+3) = vt.nor.y;
			*(offset+4) = vt.nor.z;
			
			*(offset+5) = vt.pos.x;
			*(offset+6) = vt.pos.y;
			*(offset+7) = vt.pos.z;
		}
		pr2.vertex = vertexptr;

		//add indices
		pr2.numIndices = pr1.index.size();
		GLuint *indexptr = new GLuint[pr1.index.size()];
		for(unsigned int i=0; i < pr1.index.size(); i++)
			*(indexptr + i) = pr1.index[i];
		pr2.index = indexptr;
	}

	return true;
}

void CGraphObj::unload()
{
	if(!isLoaded()) return;

	CDataObject::unload();

	//primitives:
	for(unsigned int i=0; i < m_Primitives.size(); i++)
		unloadPrimitive(m_Primitives[i]);
	m_Primitives.clear();
}

void CGraphObj::unloadPrimitive(SPrimitive &pr)
{
	delete [] (GLfloat *)pr.vertex;
	delete [] (GLuint *)pr.index;
}

//some flags for optimalisation:
bool tex_enabled;
bool use_blending;

void CGraphObj::draw(
	const SGraphicSettings *settings, CReflection *reflection,
	unsigned int lod, float t, bool useMaterials)
{
	m_CurrentSettings = settings;
	m_CurrentReflection = reflection;
	m_CurrentLOD = lod;
	m_UseMaterials = useMaterials;

	if(settings->m_EnableAnimation && fabsf(t - m_CurrentTime) > 0.01)
	{
		m_CurrentTime = t;
		animate(t);
	}
	
	//TODO: make this code very fast (it's the main drawing routine)

	use_blending = (m_CurrentSettings->m_Transparency == SGraphicSettings::blend);
	tex_enabled = true;

	for(unsigned int p=0; p < m_Primitives.size(); p++)
	{
		const SPrimitive &pr = m_Primitives[p];

		//LOD testing:
		if(lod == 4 && (pr.LODs & 8) == 0) continue;
		if(lod == 3 && (pr.LODs & 4) == 0) continue;
		if(lod == 2 && (pr.LODs & 2) == 0) continue;
		if(lod == 1 && (pr.LODs & 1) == 0) continue;

		drawPrimitive(pr);
	}

	if(!tex_enabled)
		glEnable(GL_TEXTURE_2D);

	if(!use_blending && m_CurrentSettings->m_Transparency == SGraphicSettings::blend)
		glEnable(GL_BLEND);
}


void CGraphObj::drawPrimitive(const SPrimitive &pr)
{
	CGLExtensions ext;

	glInterleavedArrays(GL_T2F_N3F_V3F, 0, pr.vertex);

	if(ext.hasCompiledVertexArray) //TODO: check if locking is a good idea
		ext.glLockArrays(0, pr.numVertices);

	//normal model
	if(setMaterial(pr, false))
		glDrawElements(GL_TRIANGLES, pr.numIndices, GL_UNSIGNED_INT, pr.index);

	//reflection
	if(m_CurrentReflection != NULL && setMaterial(pr, true))
	{
		m_CurrentReflection->enable(m_CurrentSettings);
		glDrawElements(GL_TRIANGLES, pr.numIndices, GL_UNSIGNED_INT, pr.index);
		m_CurrentReflection->disable();
		tex_enabled = true;
	}

	if(ext.hasCompiledVertexArray) //TODO: check if locking is a good idea
		ext.glUnlockArrays();
}

bool CGraphObj::setMaterial(const SPrimitive &pr, bool forReflection)
{
	//color and alpha
	float kleur[] = {1,1,1,0};

	if(forReflection)
	{
		kleur[3] = pr.reflectance;
	}
	else
	{
		if(!m_UseMaterials) return true; //don't set the material

		//Shininess:
		float speccol[] = {pr.reflectance, pr.reflectance, pr.reflectance, 0.0};
		//float speccol[] = {1,1,1, 0.0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, speccol);

		//Color:
		kleur[0] = pr.color[m_CurrentLOD-1].x;
		kleur[1] = pr.color[m_CurrentLOD-1].y;
		kleur[2] = pr.color[m_CurrentLOD-1].z;
		kleur[3] = pr.opacity;

		//texture
		if(pr.texture[m_CurrentLOD-1] == 0)
		{
			if(tex_enabled)
			{
				glDisable(GL_TEXTURE_2D);
				tex_enabled = false;
			}
		}
		else
		{
			if(!tex_enabled)
			{
				glEnable(GL_TEXTURE_2D);
				tex_enabled = true;
			}
			glBindTexture(GL_TEXTURE_2D, pr.texture[m_CurrentLOD-1]);
		}
	}

	//blending:
	if(kleur[3] < 0.1) return false; //don't draw this primitive
	if(m_CurrentSettings->m_Transparency == SGraphicSettings::blend)
	{
		if(use_blending)
		{
			if(pr.reflectance > 0.9)
			{
				use_blending = false;
				glDisable(GL_BLEND);
			}
		}
		else
		{
			if(pr.reflectance <= 0.9)
			{
				use_blending = true;
				glEnable(GL_BLEND);
			}
		}
	}

	//setting the material
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, kleur);

	return true;
}

void CGraphObj::animate(float t)
{
	for(unsigned int p=0; p < m_Primitives.size(); p++)
	{
		SPrimitive &pr = m_Primitives[p];

		if((pr.LODs & 16) != 0 && (pr.LODs & 32) != 0)
			animateWater(pr, t);

		if(pr.animation.rotationEnabled)
			animateRotation(pr, t);
	}
}

void CGraphObj::animateWater(SPrimitive &pr, float t)
{
	for(unsigned int v=0; v < pr.numVertices; v++)
	{
		GLfloat *offset = (GLfloat *)pr.vertex + 8*v;

		//GL_T2F_N3F_V3F array format:
		/*
		*(offset  ) = vt.tex.x;
		*(offset+1) = vt.tex.y;
		
		*(offset+2) = vt.nor.x;
		*(offset+3) = vt.nor.y;
		*(offset+4) = vt.nor.z;
		
		*(offset+5) = vt.pos.x;
		*(offset+6) = vt.pos.y;
		*(offset+7) = vt.pos.z;
		*/
		float x = *(offset+5);
		float z = *(offset+7);
		float wave = 0.4 * (
			sin(( x+z)*2*M_PI/TILESIZE + 2.0*t) +
			sin(( x-z)*2*M_PI/TILESIZE + 2.0*t) +
			sin((-x+z)*2*M_PI/TILESIZE + 2.0*t) +
			sin((-x-z)*2*M_PI/TILESIZE + 2.0*t)
			)
			+ 0.25 * (
			sin(( 2*x+z)*2*M_PI/TILESIZE + 5.11*t) +
			sin(( x-2*z)*2*M_PI/TILESIZE + 5.11*t) +
			sin((-x+2*z)*2*M_PI/TILESIZE + 5.11*t) +
			sin((-2*x-z)*2*M_PI/TILESIZE + 5.11*t)
			);
		*(offset+6) = m_WaterLevel + wave;
	}

	//printf("Animate water\n");
}

void CGraphObj::animateRotation(SPrimitive &pr, float t)
{
	CVector origin   = pr.animation.rotationOrigin;
	CVector velocity = pr.animation.rotationVelocity;

	CMatrix rmat; rmat.setRotation(velocity * t);

	CVector pos, nor;

	for(unsigned int v=0; v < pr.numVertices; v++)
	{
		pos = pr.originalPos[v];
		nor = pr.originalNor[v];

		pos -= origin;

		pos *= rmat;
		nor *= rmat;

		pos += origin;

		GLfloat *offset = (GLfloat *)pr.vertex + 8*v;

		//GL_T2F_N3F_V3F array format:
		/*
		*(offset  ) = vt.tex.x;
		*(offset+1) = vt.tex.y;
		
		*(offset+2) = vt.nor.x;
		*(offset+3) = vt.nor.y;
		*(offset+4) = vt.nor.z;
		
		*(offset+5) = vt.pos.x;
		*(offset+6) = vt.pos.y;
		*(offset+7) = vt.pos.z;
		*/

		*(offset+2) = nor.x;
		*(offset+3) = nor.y;
		*(offset+4) = nor.z;
		
		*(offset+5) = pos.x;
		*(offset+6) = pos.y;
		*(offset+7) = pos.z;
	}
}

