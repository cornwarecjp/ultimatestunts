/***************************************************************************
                          reflection.cpp  -  description
                             -------------------
    begin                : ma sep 20 2004
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

#include "reflection.h"
#include "camera.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdlib>
#include <cstdio>

unsigned int _reflectionTexture[6]; //textures
unsigned int _reflectionSpheremap; //display list
bool _reflectionsInitialised = false;

CReflection::CReflection(bool smooth, unsigned int size)
{
	m_Size = size;
	m_TexSmooth = smooth;

	char *data = new char[3*size*size];

	for(unsigned int i=0; i < size*size; i++)
	{
		data[3*i+0] = 255; //TODO: zero would be better when not debugging
		data[3*i+1] = 0;
		data[3*i+2] = 0;
	}

	if(!_reflectionsInitialised)
	{
		_reflectionsInitialised = true;
		initialiseReflections(data);
	}


	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glTexImage2D( GL_TEXTURE_2D, 0, 3, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	if(m_TexSmooth)
	{
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else
	{
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	}

	delete [] data;
}

CReflection::~CReflection()
{
}

void CReflection::update(CRenderer *renderer, CCamera *cam)
{
	glEnable(GL_SCISSOR_TEST);

	//set 45 degree 1:1 perspective
	GLfloat near = 0.5;
	GLfloat far = 100.0; //TILESIZE * m_VisibleTiles; //TODO

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glFrustum( -near, near, -near, near, near, far );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//set reflection size viewport
	glViewport(0, 0, m_Size/2, m_Size/2);
	glScissor(0, 0, m_Size/2, m_Size/2);


	CCamera camera[6];
	for(unsigned int i=0; i < 6; i++)
		camera[i] = *cam;

	/*
	0:front
	1:left
	2:back
	3:right
	4:top
	5:bottom
	*/

	CMatrix rotLeft, rotUp;
	rotLeft.setElement(0, 0, 0.0); rotLeft.setElement(0, 1, 0.0); rotLeft.setElement(0, 2, 1.0);
	rotLeft.setElement(1, 0, 0.0); rotLeft.setElement(1, 1, 1.0); rotLeft.setElement(1, 2, 0.0);
	rotLeft.setElement(2, 0,-1.0); rotLeft.setElement(2, 1, 0.0); rotLeft.setElement(2, 2, 0.0);

	rotUp.setElement(0, 0, 1.0); rotUp.setElement(0, 1, 0.0); rotUp.setElement(0, 2, 0.0);
	rotUp.setElement(1, 0, 0.0); rotUp.setElement(1, 1, 0.0); rotUp.setElement(1, 2, 1.0);
	rotUp.setElement(2, 0, 0.0); rotUp.setElement(2, 1,-1.0); rotUp.setElement(2, 2, 0.0);

	//left
	camera[1].setOrientation(rotLeft * camera[0].getOrientation());

	//back
	camera[2].setOrientation(rotLeft * camera[1].getOrientation());

	//right
	camera[3].setOrientation(rotLeft * camera[2].getOrientation());

	//top
	camera[4].setOrientation(rotUp * camera[0].getOrientation());

	//bottom
	camera[5].setOrientation(rotUp.transpose() * camera[0].getOrientation());


	//Draw the front reflection
	renderer->setCamera(camera);
	renderer->update();
	glBindTexture(GL_TEXTURE_2D, _reflectionTexture[0] );
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, m_Size/2, m_Size/2, 0);

	//we want to know the central pixel color
	float centralPixel[4];
	glReadPixels(m_Size/4, m_Size/4, 1, 1, GL_RGB, GL_FLOAT, centralPixel);
	
	//draw other reflections
	for(unsigned int i=1; i < 6; i++)
	{
		//render
		renderer->setCamera(camera+i);
		renderer->update();

		//copy image to texture
		glBindTexture(GL_TEXTURE_2D, _reflectionTexture[i] );
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, m_Size/2, m_Size/2, 0);
	}

	//no perspective
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	//set reflection size viewport
	glViewport(0, 0, m_Size, m_Size);
	glScissor(0, 0, m_Size, m_Size);

	float oldClear[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, oldClear);
	glClearColor(centralPixel[0],centralPixel[1],centralPixel[2],centralPixel[3]);

	bool zEnabled = glIsEnabled(GL_DEPTH_TEST);
	if(zEnabled) glDisable(GL_DEPTH_TEST);
	bool bEnabled = glIsEnabled(GL_BLEND);
	if(bEnabled) glDisable(GL_BLEND);
	bool fEnabled = glIsEnabled(GL_FOG);
	if(fEnabled) glDisable(GL_FOG);

	glClear(GL_COLOR_BUFFER_BIT);

	//draw spheremap model
	glCallList(_reflectionSpheremap);

	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, m_Size, m_Size, 0);

	glClearColor(oldClear[0],oldClear[1],oldClear[2],oldClear[3]);
	if(zEnabled) glEnable(GL_DEPTH_TEST);
	if(bEnabled) glEnable(GL_BLEND);
	if(fEnabled) glEnable(GL_FOG);

	glDisable(GL_SCISSOR_TEST);
}

void CReflection::draw() const
{
	glBindTexture(GL_TEXTURE_2D, m_Texture );
}


#define TESS 8

void drawQuads(CVector vertex[TESS+1][TESS+1])
{
	for(unsigned int s=0; s < TESS; s++)
		for(unsigned int t=0; t < TESS; t++)
		{
			float smin = 0.99*(float)(s) / (float)TESS;
			float smax = 0.99*(float)(s+1) / (float)TESS;
			float tmin = 0.99*(float)(t) / (float)TESS;
			float tmax = 0.99*(float)(t+1) / (float)TESS;
			
			glTexCoord2f(smax, tmin);
			glVertex2f(vertex[s+1][t].x, vertex[s+1][t].y);

			glTexCoord2f(smin, tmin);
			glVertex2f(vertex[s][t].x, vertex[s][t].y);

			glTexCoord2f(smin, tmax);
			glVertex2f(vertex[s][t+1].x, vertex[s][t+1].y);

			glTexCoord2f(smax, tmax);
			glVertex2f(vertex[s+1][t+1].x, vertex[s+1][t+1].y);
		}
}


void CReflection::initialiseReflections(char *data)
{
	glGenTextures(6, _reflectionTexture);

	for(unsigned int i=0; i < 6; i++)
	{
		glBindTexture(GL_TEXTURE_2D, _reflectionTexture[i]);
		glTexImage2D( GL_TEXTURE_2D, 0, 3, m_Size/2, m_Size/2, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
		if(m_TexSmooth)
		{
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		}
		else
		{
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		}
	}

	_reflectionSpheremap = glGenLists(1);
	glNewList(_reflectionSpheremap, GL_COMPILE);

	glDisable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	CVector vertex[TESS+1][TESS+1];

	//left
	for(unsigned int s=0; s < TESS+1; s++)
		for(unsigned int t=0; t < TESS+1; t++)
		{
			vertex[s][t].x = 1.0; 
			vertex[s][t].y = 2.0*(float)t / (float)TESS - 1.0;
			vertex[s][t].z = 2.0*(float)s / (float)TESS - 1.0;
			vertex[s][t].normalise();
			vertex[s][t] += CVector(0,0,1);
			vertex[s][t].normalise();
		}

	glBindTexture(GL_TEXTURE_2D, _reflectionTexture[1]);
	glBegin(GL_QUADS);
	drawQuads(vertex);
	glEnd();

	//back
	for(unsigned int s=0; s < TESS+1; s++)
		for(unsigned int t=0; t < TESS+1; t++)
		{
			vertex[s][t].x = -(2.0*(float)s / (float)TESS - 1.0);
			vertex[s][t].y = 2.0*(float)t / (float)TESS - 1.0;
			vertex[s][t].z = 1.0;
			vertex[s][t].normalise();
			vertex[s][t] += CVector(0,0,1);
			vertex[s][t].normalise();
		}

	glBindTexture(GL_TEXTURE_2D, _reflectionTexture[2]);
	glBegin(GL_QUADS);
	drawQuads(vertex);
	glEnd();

	//right
	for(unsigned int s=0; s < TESS+1; s++)
		for(unsigned int t=0; t < TESS+1; t++)
		{
			vertex[s][t].x = -1.0;
			vertex[s][t].y = 2.0*(float)t / (float)TESS - 1.0;
			vertex[s][t].z = -(2.0*(float)s / (float)TESS - 1.0);
			vertex[s][t].normalise();
			vertex[s][t] += CVector(0,0,1);
			vertex[s][t].normalise();
		}

	glBindTexture(GL_TEXTURE_2D, _reflectionTexture[3]);
	glBegin(GL_QUADS);
	drawQuads(vertex);
	glEnd();

	//top
	for(unsigned int s=0; s < TESS+1; s++)
		for(unsigned int t=0; t < TESS+1; t++)
		{
			vertex[s][t].x = 2.0*(float)s / (float)TESS - 1.0;
			vertex[s][t].y = 1.0; 
			vertex[s][t].z = 2.0*(float)t / (float)TESS - 1.0;
			vertex[s][t].normalise();
			vertex[s][t] += CVector(0,0,1);
			vertex[s][t].normalise();
		}

	glBindTexture(GL_TEXTURE_2D, _reflectionTexture[4]);
	glBegin(GL_QUADS);
	drawQuads(vertex);
	glEnd();

	//bottom
	for(unsigned int s=0; s < TESS+1; s++)
		for(unsigned int t=0; t < TESS+1; t++)
		{
			vertex[s][t].x = 2.0*(float)s / (float)TESS - 1.0;
			vertex[s][t].y = -1.0;
			vertex[s][t].z = -(2.0*(float)t / (float)TESS - 1.0);
			vertex[s][t].normalise();
			vertex[s][t] += CVector(0,0,1);
			vertex[s][t].normalise();
		}

	glBindTexture(GL_TEXTURE_2D, _reflectionTexture[5]);
	glBegin(GL_QUADS);
	drawQuads(vertex);
	glEnd();

#define MIN_T (TESS/8+1)

	//front: special
	glBindTexture(GL_TEXTURE_2D, _reflectionTexture[0]);
	glBegin(GL_QUADS);

	//front bottom
	for(unsigned int s=0; s < TESS+1; s++)
		for(unsigned int t=MIN_T; t < TESS+1; t++)
		{
			float d = (float)t / (float)TESS;
			vertex[s][t].x = (2.0*(float)s / (float)TESS - 1.0) * d;
			vertex[s][t].y = -d;
			vertex[s][t].z = -1.0; 
			vertex[s][t].normalise();
			vertex[s][t] += CVector(0,0,1);
			vertex[s][t].normalise();
		}

	for(unsigned int s=0; s < TESS; s++)
		for(unsigned int t=MIN_T; t < TESS; t++)
		{
			float d = (float)t / (float)TESS;
			float smin = 0.5*(1-d) + d*(float)(s) / (float)TESS;
			float smax = 0.5*(1-d) + d*(float)(s+1) / (float)TESS;
			float tmin = 0.5 - 0.5*(float)(t) / (float)TESS;
			float tmax = 0.5 - 0.5*(float)(t+1) / (float)TESS;

			glTexCoord2f(smin, tmin);
			glVertex2f(vertex[s][t].x, vertex[s][t].y);

			glTexCoord2f(smax, tmin);
			glVertex2f(vertex[s+1][t].x, vertex[s+1][t].y);

			glTexCoord2f(smax, tmax);
			glVertex2f(vertex[s+1][t+1].x, vertex[s+1][t+1].y);

			glTexCoord2f(smin, tmax);
			glVertex2f(vertex[s][t+1].x, vertex[s][t+1].y);
		}

	//front top
	for(unsigned int s=0; s < TESS+1; s++)
		for(unsigned int t=MIN_T; t < TESS+1; t++)
		{
			float d = (float)t / (float)TESS;
			vertex[s][t].x = (2.0*(float)s / (float)TESS - 1.0) * d;
			vertex[s][t].y = d;
			vertex[s][t].z = -1.0;
			vertex[s][t].normalise();
			vertex[s][t] += CVector(0,0,1);
			vertex[s][t].normalise();
		}

	for(unsigned int s=0; s < TESS; s++)
		for(unsigned int t=MIN_T; t < TESS; t++)
		{
			float d = (float)t / (float)TESS;
			float smin = 0.5*(1-d) + d*(float)(s) / (float)TESS;
			float smax = 0.5*(1-d) + d*(float)(s+1) / (float)TESS;
			float tmin = 0.5 + 0.5*(float)(t) / (float)TESS;
			float tmax = 0.5 + 0.5*(float)(t+1) / (float)TESS;

			glTexCoord2f(smax, tmin);
			glVertex2f(vertex[s+1][t].x, vertex[s+1][t].y);

			glTexCoord2f(smin, tmin);
			glVertex2f(vertex[s][t].x, vertex[s][t].y);

			glTexCoord2f(smin, tmax);
			glVertex2f(vertex[s][t+1].x, vertex[s][t+1].y);

			glTexCoord2f(smax, tmax);
			glVertex2f(vertex[s+1][t+1].x, vertex[s+1][t+1].y);
		}

	//front left
	for(unsigned int s=0; s < TESS+1; s++)
		for(unsigned int t=MIN_T; t < TESS+1; t++)
		{
			float d = (float)t / (float)TESS;

			vertex[s][t].x = -d;
			vertex[s][t].y = (2.0*(float)s / (float)TESS - 1.0) * d;
			vertex[s][t].z = -1.0;
			vertex[s][t].normalise();
			vertex[s][t] += CVector(0,0,1);
			vertex[s][t].normalise();
		}

	for(unsigned int s=0; s < TESS; s++)
		for(unsigned int t=MIN_T; t < TESS; t++)
		{
			float d = (float)t / (float)TESS;
			float smin = 0.5*(1-d) + d*(float)(s) / (float)TESS;
			float smax = 0.5*(1-d) + d*(float)(s+1) / (float)TESS;
			float tmin = 0.5 - 0.5*(float)(t) / (float)TESS;
			float tmax = 0.5 - 0.5*(float)(t+1) / (float)TESS;

			glTexCoord2f(tmin, smax);
			glVertex2f(vertex[s+1][t].x, vertex[s+1][t].y);

			glTexCoord2f(tmin, smin);
			glVertex2f(vertex[s][t].x, vertex[s][t].y);

			glTexCoord2f(tmax, smin);
			glVertex2f(vertex[s][t+1].x, vertex[s][t+1].y);

			glTexCoord2f(tmax, smax);
			glVertex2f(vertex[s+1][t+1].x, vertex[s+1][t+1].y);
		}

	//front right
	for(unsigned int s=0; s < TESS+1; s++)
		for(unsigned int t=MIN_T; t < TESS+1; t++)
		{
			float d = (float)t / (float)TESS;

			vertex[s][t].x = d;
			vertex[s][t].y = (2.0*(float)s / (float)TESS - 1.0) * d;
			vertex[s][t].z = -1.0;
			vertex[s][t].normalise();
			vertex[s][t] += CVector(0,0,1);
			vertex[s][t].normalise();
		}

	for(unsigned int s=0; s < TESS; s++)
		for(unsigned int t=MIN_T; t < TESS; t++)
		{
			float d = (float)t / (float)TESS;
			float smin = 0.5*(1-d) + d*(float)(s) / (float)TESS;
			float smax = 0.5*(1-d) + d*(float)(s+1) / (float)TESS;
			float tmin = 0.5 + 0.5*(float)(t) / (float)TESS;
			float tmax = 0.5 + 0.5*(float)(t+1) / (float)TESS;

			glTexCoord2f(tmin, smin);
			glVertex2f(vertex[s][t].x, vertex[s][t].y);

			glTexCoord2f(tmin, smax);
			glVertex2f(vertex[s+1][t].x, vertex[s+1][t].y);

			glTexCoord2f(tmax, smax);
			glVertex2f(vertex[s+1][t+1].x, vertex[s+1][t+1].y);

			glTexCoord2f(tmax, smin);
			glVertex2f(vertex[s][t+1].x, vertex[s][t+1].y);
		}

	glEnd();

	glEnable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glEndList();
}
