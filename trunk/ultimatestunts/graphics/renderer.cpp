/***************************************************************************
                          renderer.cpp  -  The graphics rendering routines
                             -------------------
    begin                : di jan 28 2003
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
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>

#include "renderer.h"
#include "usmacros.h"

//TODO: put this into the class
int camx, camy, camz;
int lengte, breedte, hoogte;

CRenderer::CRenderer(const CLConfig &conf, const CWorld *world)
{
	m_World = world;
	m_GraphicWorld = new CGraphicWorld(world, conf);
	m_FogColor = new float[4];

	//Default values:
	m_UseBackground = true;
	m_ZBuffer = true;
	m_VisibleTiles = 10;
	m_FogMode = GL_EXP;
	m_TexPerspective = true;
	m_TexSmooth = true;
	m_ShadowSmooth = true;

	//Load the setings
	CString cnf = conf.getValue("graphics", "background_size");
	//printf("Background size: %s\n", cnf.c_str());
	if(cnf != "" && cnf.toInt() <= 4)
		m_UseBackground = false;

	cnf = conf.getValue("graphics", "visible_tiles");
	//printf("Visible tiles: %s\n", cnf.c_str());
	if(cnf != "")
		m_VisibleTiles = cnf.toInt();

	cnf = conf.getValue("graphics", "texture_perspective");
	//printf("Texture perspective: %s\n", cnf.c_str());
	m_TexPerspective = (cnf != "false");

	cnf = conf.getValue("graphics", "zbuffer");
	//printf("Z buffer: %s\n", cnf.c_str());
	m_ZBuffer = (cnf != "false");

	cnf = conf.getValue("graphics", "texture_smooth");
	//printf("Smooth textures: %s\n", cnf.c_str());
	m_TexSmooth = (cnf != "false");

	cnf = conf.getValue("graphics", "shadows_smooth");
	//printf("Smooth shadows: %s\n", cnf.c_str());
	m_ShadowSmooth = (cnf != "false");

	cnf = conf.getValue("graphics", "fogmode");
	//printf("Fog mode: %s\n", cnf.c_str());
	if(cnf != "")
	{
		if(cnf == "off")		m_FogMode = -1;
		if(cnf == "linear")	m_FogMode = GL_LINEAR;
		if(cnf == "exp")		m_FogMode = GL_EXP;
		if(cnf == "exp2")	m_FogMode = GL_EXP2;
	}

	//Next: use these settings
	if(m_ZBuffer)
		{glEnable(GL_DEPTH_TEST);}
	else
		{glDisable(GL_DEPTH_TEST);}

	if(m_FogMode  < 0)
		{glDisable(GL_FOG);}
	else
	{
		glEnable(GL_FOG);
		glFogi(GL_FOG_MODE, m_FogMode);
		glFogf(GL_FOG_DENSITY, 2.0/(float)(TILESIZE*m_VisibleTiles));
		glFogi(GL_FOG_START, 0);
		glFogi(GL_FOG_END, TILESIZE*m_VisibleTiles);
	}

	if(m_TexSmooth)
	{
		glHint( GL_POINT_SMOOTH_HINT, GL_NICEST );
		glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	}
	else
	{
		glHint( GL_POINT_SMOOTH_HINT, GL_FASTEST );
		glHint( GL_POLYGON_SMOOTH_HINT, GL_FASTEST );
	}

	if(m_TexPerspective)
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	else
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	if(m_ShadowSmooth)
		glShadeModel( GL_SMOOTH );
	else
		glShadeModel( GL_FLAT );

	//And some other settings:
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
}

CRenderer::~CRenderer()
{
	unloadTrackData();
	delete m_GraphicWorld;
	delete [] m_FogColor;
}

bool CRenderer::loadTrackData()
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

void CRenderer::unloadTrackData()
{
	m_GraphicWorld->unloadWorld();
}

bool CRenderer::loadObjData()
{
	return m_GraphicWorld->loadObjects();
}

void CRenderer::unloadObjData()
{
	m_GraphicWorld->unloadObjects();
}

void CRenderer::setCamera(const CCamera *cam)
{m_Camera = cam;}

void CRenderer::update()
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
		{   //setColor(red);
			if (z >= 0) viewTrackPart(xmin,zmin, x,z, 1,1, y); //voor
			if (z <= breedte) viewTrackPart(xmin,zmax-1, x,z, 1,-1, y); //achter
			if (z<breedte && z>=0) viewTrackPart(xmin,z, x,z+1, 1,1, y); //linker strook
		}
		if (x <= lengte) //Rechter deel
		{   //setColor(green);
			if (z >= 0) viewTrackPart(xmax-1,zmin, x,z, -1,1, y); //voor
			if (z <= breedte) viewTrackPart(xmax-1,zmax-1, x,z, -1,-1, y); //achter
			if (z<breedte && z>=0) viewTrackPart(xmax-1,z, x,z+1, -1,1, y); //rechter strook
		}

		//setColor(blue);
		if (z >= 0 && x<lengte && x>=0) viewTrackPart(x,zmin, x+1,z, 1,1, y); //voor strook
		if (z <= breedte && x<lengte && x>=0) viewTrackPart(x,zmax-1, x+1,z, 1,-1, y); //achter strook

		glTranslatef(x * TILESIZE, 0, z * TILESIZE);

		if (x>=0 && z>=0 && x<lengte && z<breedte) viewPilaar(x, z, y);
	}
	else //zbuffer
	{
		viewTrackPart(xmin, zmin, xmax, zmax, 1, 1, y);
	}

	glPopMatrix();

	//Tijdelijke plaats om auto's te renderen: achteraan
	int num_objs = m_World->m_MovObjs.size();
	for(int i=0; i<num_objs; i++)
	{
		CMovingObject *mo = m_World->m_MovObjs[i];

		switch(mo->getType())
		{
			case CMessageBuffer::car:
				viewCar((CCar *)mo);
				break;
			default:
				printf("Warning: could not render unknown object type\n");
		}
	}
}

void CRenderer::viewBackground()
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

void CRenderer::viewCar(CCar *car)
{
	glPushMatrix();

	CVector r = car->getPosition();
	const CMatrix &m = car->getRotationMatrix();

	//printf("Drawing a car at position %f,%f,%f\n", r.x,r.y,r.z);

	glTranslatef (r.x, r.y, r.z);
	glMultMatrixf(m.gl_mtr());

	m_GraphicWorld->m_MovingObjects[car->m_Bound].draw(1); //TODO: LOD
	//TODO: other CBound members

	glPopMatrix();
}

void CRenderer::viewTrackPart(
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

void CRenderer::viewPilaar(int x, int y, int cur_zpos)
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
					//((CGraphicShape *)(temp.m_Shape))->draw(lod);
					m_GraphicWorld->m_Tiles[temp.m_Shape].draw(lod);
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
			//((CGraphicShape *)(temp.m_Shape))->draw(lod);
			m_GraphicWorld->m_Tiles[temp.m_Shape].draw(lod);
		}

	glPopMatrix();
}
