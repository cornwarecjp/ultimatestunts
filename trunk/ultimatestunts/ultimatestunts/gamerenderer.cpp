/***************************************************************************
                          gamerenderer.cpp  -  Renderer using world object
                             -------------------
    begin                : di apr 8 2003
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
#include <cstdio>
#include "gamerenderer.h"

CGameRenderer::CGameRenderer(const CLConfig &conf, const CWorld *world) : CRenderer(conf)
{
	m_World = world;
	m_GraphicWorld = new CGraphicWorld(world, conf);
}

CGameRenderer::~CGameRenderer()
{
	unloadTrackData();
	delete m_GraphicWorld;
}

bool CGameRenderer::loadTrackData()
{
	if(m_GraphicWorld->loadWorld())
	{
		lengte = m_World->m_L;
		breedte = m_World->m_W;
		hoogte = m_World->m_H;

		CVector fc = m_GraphicWorld->m_Background.getColor();
		m_FogColor[0] = fc.x;
		m_FogColor[1] = fc.y;
		m_FogColor[2] = fc.z;
		m_FogColor[3] = 1.0;
		glClearColor(	m_FogColor[0],m_FogColor[1],m_FogColor[2],m_FogColor[3]);

		if(m_FogMode >= 0)
			glFogfv(GL_FOG_COLOR, m_FogColor);

		return true;
	}

	return false;
}

void CGameRenderer::unloadTrackData()
{
	m_GraphicWorld->unloadWorld();
}

bool CGameRenderer::loadObjData()
{
	return m_GraphicWorld->loadObjects();
}

void CGameRenderer::unloadObjData()
{
	m_GraphicWorld->unloadObjects();
}

void CGameRenderer::update()
{

	if(m_ZBuffer)
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	else
		glClear( GL_COLOR_BUFFER_BIT );

	const CMatrix &cammat = m_Camera->getOrientation();
	//glLoadIdentity();
	glLoadMatrixf(cammat.inverse().gl_mtr());

	if(m_UseBackground)
	{
		viewBackground();
		if(m_ZBuffer)
			glClear( GL_DEPTH_BUFFER_BIT );
	}

	//Lighting:
	GLfloat sun_diffuse[] = {0.9, 1.0, 0.75, 1.0};  /* Yellow 'sun' diffuse light. */
	GLfloat sun_position[] = {0.69, 0.2, 0.7, 0.0};  /* Infinite light location. */

	glLightfv(GL_LIGHT0, GL_POSITION, sun_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_diffuse);

	const CVector &camera = m_Camera->getPosition();

	glTranslatef (-camera.x, -camera.y, -camera.z);


	glPushMatrix();

	//Nu volgt de weergave-routine

	int x = (int)(0.5 + (camera.x)/TILESIZE);
	int y = (int)(0.5 + (camera.y)/VERTSIZE);
	int z = (int)(0.5 + (camera.z)/TILESIZE);

	camx=x; camy=y; camz=z;
	//printf ("x,y,z = %d,%d,%d\n",x,y,z);

	//Nu: de dynamische begrenzing om weergavesnelheid te vergroten
	int xmin = (x-m_VisibleTiles < 0)? 0 : x-m_VisibleTiles;
	int xmax = (x+m_VisibleTiles > lengte)? lengte : x+m_VisibleTiles;
	int zmin = (z-m_VisibleTiles < 0)? 0 : z-m_VisibleTiles;
	int zmax = (z+m_VisibleTiles > breedte)? breedte : z+m_VisibleTiles;

	if(!m_ZBuffer)
	{
		if (x >= 0) //Linker deel
		{
			if (z <= breedte) viewTrackPart(xmin,zmax-1, x,z, 1,-1, y); //achter
			if (z >= 0) viewTrackPart(xmin,zmin, x,z+1, 1,1, y); //voor+linker strook
		}
		if (x <= lengte) //Rechter deel
		{
			if (z <= breedte) viewTrackPart(xmax-1,zmax-1, x-1,z, -1,-1, y); //achter
			if (z >= 0) viewTrackPart(xmax-1,zmin, x-1,z+1, -1,1, y); //voor+rechter strook
		}

	}
	else //zbuffer
	{
		//printf("Using z-buffering\n");
		viewTrackPart(xmin, zmin, xmax, zmax, 1, 1, y);
	}

	glPopMatrix();

	//Tijdelijke plaats om auto's te renderen: achteraan
	int num_objs = m_World->m_MovObjs.size();
	for(int i=0; i<num_objs; i++)
		viewMovObj(m_World->m_MovObjs[i]);

}

void CGameRenderer::viewBackground()
{
	if(m_FogMode != -1)
		glDisable(GL_FOG);
	if(m_ZBuffer)
		glDisable(GL_DEPTH_TEST);

	m_GraphicWorld->m_Background.draw();

	if(m_FogMode != -1)
		glEnable(GL_FOG);
	if(m_ZBuffer)
		glEnable(GL_DEPTH_TEST);
}

void CGameRenderer::viewMovObj(CMovingObject *mo)
{
	glPushMatrix();

	CVector r = mo->getPosition();
	const CMatrix &m = mo->getOrientation();
	//printf("Drawing a car at position %f,%f,%f\n", r.x,r.y,r.z);

	glTranslatef (r.x, r.y, r.z);
	glMultMatrixf(m.gl_mtr());

	for(unsigned int i=mo->m_Bodies.size(); i > 0; i--) //TODO: depth sorting?
	{
		glPushMatrix();

		CBody &b = mo->m_Bodies[i-1];
		r = b.m_Position;
		glTranslatef (r.x, r.y, r.z);
		glMultMatrixf(b.m_Orientation.gl_mtr());
		m_GraphicWorld->m_MovingObjects[b.m_Body].draw(1); //TODO: LOD

		glPopMatrix();
	}

	glPopMatrix();
}

void CGameRenderer::viewTrackPart(
	int xmin,int ymin,
	int xmax,int ymax,
	int dx,  int dy,
	int cur_zpos)
{
	glPushMatrix();
	glTranslatef(xmin * TILESIZE, 0, ymin * TILESIZE);

	if(xmin<0) xmin=0;
	if(ymin<0) ymin=0;
	if(xmin>=lengte) xmin=lengte-1;
	if(ymin>=breedte) ymin=breedte-1;
	if(xmax<-1) xmax=-1;
	if(ymax<-1) ymax=-1;
	if(xmax>lengte) xmax=lengte;
	if(ymax>breedte) ymax=breedte;

	for (int i = xmin; dx*i < dx*xmax; i+=dx)
	{
		glPushMatrix();
		for (int j = ymin; dy*j < dy*ymax; j+=dy)
		{
			viewPilaar(i, j, cur_zpos);
			glTranslatef(0, 0, dy * TILESIZE);
		}
		glPopMatrix();
		glTranslatef(dx * TILESIZE, 0, 0);
	}

	glPopMatrix();
}

void CGameRenderer::viewPilaar(int x, int y, int cur_zpos)
{
	glPushMatrix();

		int lod;
		int d=abs(camx-x);
		int dy=abs(camz-y); //Dit (camz) is geen typefout!!!
		if(dy>d) d=dy; //d is de grootste van de 2

		if(d>7)
			lod = 4;
		else if(d>3)
			lod = 3;
		else if(d>1)
			lod = 2;
		else
			lod = 1;

		int pilaar_index = hoogte * y + hoogte * breedte * x;
		int ynu = 0;
		int rnu = 0;

		for (int i = 0; i < hoogte; i++) //van beneden naar boven
		{
			CTile temp = m_World->m_Track[pilaar_index + i]; //welke tile?

			//goede hoogte
			int ystrax = temp.m_Z;

			if(ystrax >= camy)
			{
				for(int j = hoogte-1; j >= i; j--) //van boven naar beneden
				{
					temp = m_World->m_Track[pilaar_index + j]; //welke tile?

					//goede hoogte
					ystrax = temp.m_Z;

					glTranslatef(0, VERTSIZE*(ystrax-ynu),0);
					ynu = ystrax;

					//goede orientatie
					int rstrax = temp.m_R;
					if (rstrax != rnu)
					{
						glRotatef(90*(rstrax-rnu),0,1,0);
						rnu = rstrax;
					}

					//tekenen
					m_GraphicWorld->m_Tiles[temp.m_Model].draw(lod);
				}
				break;
			}

			glTranslatef(0, VERTSIZE*(ystrax-ynu),0);
			ynu = ystrax;

			//goede orientatie
			int rstrax = temp.m_R;
			if (rstrax != rnu)
			{
				glRotatef(90*(rstrax-rnu),0,1,0);
				rnu = rstrax;
			}

			//tekenen
			m_GraphicWorld->m_Tiles[temp.m_Model].draw(lod);
		}

	glPopMatrix();
}
