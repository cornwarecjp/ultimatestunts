/***************************************************************************
                          objectviewer.cpp  -  Viewing CGraphObj objects
                             -------------------
    begin                : wo sep 06 2006
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
#include <cmath>
#include <GL/gl.h>

#include "objectviewer.h"
#include "usmacros.h"

#include "graphobj.h"
#include "texture.h"

CObjectViewer::CObjectViewer(const CWinSystem *winsys, CGraphicWorld *world) : CRenderer(winsys)
{
	m_World = world;
	m_Reflection = NULL;
}

CObjectViewer::~CObjectViewer()
{
	if(m_Reflection != NULL) delete m_Reflection;
}

void CObjectViewer::setReflection(const CString &filename)
{
	if(m_Reflection != NULL) delete m_Reflection;

	CStaticReflection *ref = new CStaticReflection(NULL); //not managed by a data manager
	ref->load(filename, CParamList());
	m_Reflection = ref;
}

void CObjectViewer::addObject(
	const CString &filename, CParamList list, CVector pos,
	CMatrix ori, bool reflect, int replace)
{
	if(replace >= 0 && replace < int(m_Objects.size()))
	{
		//Re-use object

		reloadData();
		CGraphObj *obj = m_World->getMovObjBound(m_Objects[replace].objectID);
		if(obj != NULL) obj->load(filename, list); //re-use CGraphObj object, if existing

		m_Objects[replace].filename = filename;
		m_Objects[replace].parameters = list;
		m_Objects[replace].position = pos;
		m_Objects[replace].orientation = ori;
		m_Objects[replace].reflectInGround = reflect;
		reloadData();
	}
	else
	{
		//Add new object
		SObject obj;
		obj.filename = filename;
		obj.parameters = list;
		obj.position = pos;
		obj.orientation = ori;
		obj.reflectInGround = reflect;
		m_Objects.push_back(obj);
		reloadData();
	}
}

void CObjectViewer::addTexture(const CString &filename)
{
	STexture tex;
	tex.filename = filename;
	m_Textures.push_back(tex);
	reloadData();
}

void CObjectViewer::update()
{
	//set up viewport
	CRenderer::update();

	//printf("Updating graphics\n");

	//Clear the screen
	/*
	if(m_Settings.m_ZBuffer)
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	else
		glClear( GL_COLOR_BUFFER_BIT );
	*/

	//Lighting:
	GLfloat light_color[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat specular_color[] = {3.0, 3.0, 3.0, 1.0};
	GLfloat ambient_color[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat light_direction[] = {0.0, 0.0, 1.0, 0.0};

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular_color);
	glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_color);


	//Set the camera
	const CMatrix &cammat = m_Camera->getOrientation();
	glLoadMatrixf(cammat.transpose().gl_mtr());
	const CVector &camera = m_Camera->getPosition();
	glTranslatef (-camera.x, -camera.y, -camera.z);

	glColor3f(1,1,1);

	glPushMatrix();
	glScalef(1.0,-1.0,1.0);
	glCullFace(GL_FRONT);

	//The reflection
	viewObjects(true);

	glCullFace(GL_BACK); //back to default
	glPopMatrix();

	//The object itself:
	viewObjects(false);
}

void CObjectViewer::viewObjects(bool isReflection)
{
	for(unsigned int i=0; i < m_Objects.size(); i++)
	{
		CGraphObj *obj = m_World->getMovObjBound(m_Objects[i].objectID);
		if(obj == NULL) continue; //error

		if(isReflection && !(m_Objects[i].reflectInGround))
			continue; //not in reflection

		glPushMatrix();
		glTranslatef(m_Objects[i].position.x, m_Objects[i].position.y, m_Objects[i].position.z);
		glMultMatrixf(m_Objects[i].orientation.gl_mtr());

		if(m_Settings.m_Transparency == SGraphicSettings::off)
		{
			obj->draw(&m_Settings, NULL, 1, 0.0, true);
		}
		else
		{
			obj->draw(&m_Settings, m_Reflection, 1, 0.0, true);
		}

		glPopMatrix();
	}
}

void CObjectViewer::updateScreenSize()
{
	; //Don't update screen size: it is filled in from the outside
}

void CObjectViewer::reloadData()
{
	for(unsigned int i=0; i < m_Textures.size(); i++)
	{
		m_Textures[i].textureID = m_World->loadObject(
			m_Textures[i].filename, CParamList(), CDataObject::eMaterial);
	}

	for(unsigned int i=0; i < m_Objects.size(); i++)
	{
		m_Objects[i].objectID = m_World->loadObject(
			m_Objects[i].filename, m_Objects[i].parameters, CDataObject::eBound);
	}
}
