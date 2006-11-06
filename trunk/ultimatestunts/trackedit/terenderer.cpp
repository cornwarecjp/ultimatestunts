/***************************************************************************
                          terenderer.cpp  -  Rendering class of the track editor
                             -------------------
    begin                : ma mei 23 2005
    copyright            : (C) 2005 by CJP
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

#include "terenderer.h"
#include "tecamera.h"
#include "usmacros.h"
#include "datafile.h"

CTERenderer::CTERenderer(const CWinSystem *winsys) : CRenderer(winsys)
{
	m_Manager = NULL;
	camx = camy = camz = 0;
	tgtx = tgty = tgtz = 0;

	m_X = m_Y = m_W = m_H = 0;
}

CTERenderer::~CTERenderer()
{
}

void CTERenderer::setManager(CTEManager *manager)
{
	m_Manager = manager;
	m_Settings.m_VisibleTiles = 100; //make sure that a lot of tiles are visible
}

void CTERenderer::updateScreenSize()
{
	; //Don't update screen size: it is filled in from the outside
}

void CTERenderer::update()
{
	//set up viewport
	CRenderer::update();

	//printf("Updating graphics\n");

	//Clear the screen
	if(m_Settings.m_ZBuffer)
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	else
		glClear( GL_COLOR_BUFFER_BIT );

	//Set the camera
	const CMatrix &cammat = m_Camera->getOrientation();
	glLoadMatrixf(cammat.transpose().gl_mtr());
	const CVector &camera = m_Camera->getPosition();
	glTranslatef (-camera.x, -camera.y, -camera.z);

	camx = (int)(0.5 + (camera.x)/TILESIZE);
	camy = (int)(0.5 + (camera.y)/VERTSIZE);
	camz = (int)(0.5 + (camera.z)/TILESIZE);
	CVector target = ((CTECamera *)m_Camera)->getTargetPos();
	tgtx = (int)(0.5 + (target.x)/TILESIZE);
	tgty = (int)(0.5 + (target.y)/VERTSIZE);
	tgtz = (int)(0.5 + (target.z)/TILESIZE);
	//printf ("x,y,z = %d,%d,%d\n",camx,camy,camz);

	glColor3f(1,1,1);

	//Draw the track
	glDisable(GL_FOG);
	drawTrack();

	//Some line graphics
	float color[] = {1.0, 1.0, 1.0, 1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	//The routes
	vector<CTrack::CRoute> &routes = m_Manager->getTrack()->m_Routes;
	for(unsigned int r=0; r < routes.size(); r++)
	{
		if(routes[r].size() < 2) continue;

		color[2] = 1.0 * !(r & 1);
		color[0] = 1.0 * !(r & 2);
		color[1] = 1.0 * !(r & 4);
		glColor3f(color[0], color[1], color[2]);

		glBegin(GL_LINES);

		for(unsigned int i=1; i < routes[r].size(); i++)
		{
			CVector pos1(
				TILESIZE*routes[r][i-1].x, VERTSIZE*routes[r][i-1].y, TILESIZE*routes[r][i-1].z);
			CVector pos2(
				TILESIZE*routes[r][i].x, VERTSIZE*routes[r][i].y, TILESIZE*routes[r][i].z);

			pos1 += CVector(0.0, 0.5*VERTSIZE, 0.0); //in the middle of the tile
			pos2 += CVector(0.0, 0.5*VERTSIZE, 0.0); //in the middle of the tile

			glVertex3f(pos1.x, pos1.y, pos1.z);
			glVertex3f(pos2.x, pos2.y, pos2.z);

			glVertex3f(pos2.x-2.0, pos2.y, pos2.z);
			glVertex3f(pos2.x+2.0, pos2.y, pos2.z);
			glVertex3f(pos2.x, pos2.y-2.0, pos2.z);
			glVertex3f(pos2.x, pos2.y+2.0, pos2.z);
			glVertex3f(pos2.x, pos2.y, pos2.z-2.0);
			glVertex3f(pos2.x, pos2.y, pos2.z+2.0);
		}

		glEnd();
	}

	glColor3f(1,1,1); //back to white

	//The coordinate axes
	glBegin(GL_LINES);

	glVertex3f(-TILESIZE/2,0.0,0.0);
	glVertex3f(TILESIZE/2,0.0,0.0);

	glVertex3f(0.0,0.0,-TILESIZE/2);
	glVertex3f(0.0,0.0,TILESIZE/2);

	glVertex3f(0.0,0.0,0.0);
	glVertex3f(0.0,VERTSIZE,0.0);

	glEnd();

	
	//The active tile
	int  lengte = m_Manager->getTrack()->m_L;
	int  breedte = m_Manager->getTrack()->m_W;
	if(tgtx>=0 && tgtx<lengte && tgtz>=0 && tgtz<breedte)
	{
		int hoogte = m_Manager->getTrack()->m_H;
		int pilaar_index = hoogte * tgtz + hoogte * breedte * tgtx;

		int ymin = 1000, ymax = -1000;
		for (int i = 0; i < hoogte; i++) //bottom to top
		{
			STile temp = m_Manager->getTrack()->m_Track[pilaar_index + i]; //welke tile?

			if(temp.m_Model == 0) break; //0 = empty tile

			if(temp.m_Z > ymax) ymax = temp.m_Z;
			if(temp.m_Z < ymin) ymin = temp.m_Z;
		}
		ymax++;

		glPushMatrix();
		glTranslatef(TILESIZE*tgtx, 0.0, TILESIZE*tgtz);

		glBegin(GL_LINE_LOOP);

		glVertex3f(-TILESIZE/2,VERTSIZE*ymin,-TILESIZE/2);
		glVertex3f(-TILESIZE/2,VERTSIZE*ymin, TILESIZE/2);
		glVertex3f( TILESIZE/2,VERTSIZE*ymin, TILESIZE/2);
		glVertex3f( TILESIZE/2,VERTSIZE*ymin,-TILESIZE/2);

		glEnd();

		glBegin(GL_LINE_LOOP);

		glVertex3f(-TILESIZE/2,VERTSIZE*ymax,-TILESIZE/2);
		glVertex3f(-TILESIZE/2,VERTSIZE*ymax, TILESIZE/2);
		glVertex3f( TILESIZE/2,VERTSIZE*ymax, TILESIZE/2);
		glVertex3f( TILESIZE/2,VERTSIZE*ymax,-TILESIZE/2);

		glEnd();

		glBegin(GL_LINES);

		glVertex3f(-TILESIZE/2,VERTSIZE*ymin,-TILESIZE/2);
		glVertex3f(-TILESIZE/2,VERTSIZE*ymax,-TILESIZE/2);
		glVertex3f(-TILESIZE/2,VERTSIZE*ymin, TILESIZE/2);
		glVertex3f(-TILESIZE/2,VERTSIZE*ymax, TILESIZE/2);
		glVertex3f( TILESIZE/2,VERTSIZE*ymin, TILESIZE/2);
		glVertex3f( TILESIZE/2,VERTSIZE*ymax, TILESIZE/2);
		glVertex3f( TILESIZE/2,VERTSIZE*ymin,-TILESIZE/2);
		glVertex3f( TILESIZE/2,VERTSIZE*ymax,-TILESIZE/2);

		glEnd();

		glPopMatrix();
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}

void CTERenderer::drawTrack()
{
	glPushMatrix();

	//Nu volgt de weergave-routine
	int lengte = m_Manager->getTrack()->m_L;
	int  breedte = m_Manager->getTrack()->m_W;

	//Nu: de dynamische begrenzing om weergavesnelheid te vergroten
	/*
	int xmin = (camx-m_Settings.m_VisibleTiles < 0)? 0 : camx-m_Settings.m_VisibleTiles;
	int xmax = (camx+m_Settings.m_VisibleTiles > lengte)? lengte : camx+m_Settings.m_VisibleTiles;
	int zmin = (camz-m_Settings.m_VisibleTiles < 0)? 0 : camz-m_Settings.m_VisibleTiles;
	int zmax = (camz+m_Settings.m_VisibleTiles > breedte)? breedte : camz+m_Settings.m_VisibleTiles;
	*/
	int xmin = 0;
	int xmax = lengte;
	int zmin = 0;
	int zmax = breedte;

	if (camx >= 0) //Linker deel
	{
		if (camz <= breedte) viewTrackPart(xmin,zmax-1, camx,camz, 1,-1, camy); //achter
		if (camz >= 0) viewTrackPart(xmin,zmin, camx,camz+1, 1,1, camy); //voor+linker strook
	}
	if (camx <= lengte) //Rechter deel
	{
		if (camz <= breedte) viewTrackPart(xmax-1,zmax-1, camx-1,camz, -1,-1, camy); //achter
		if (camz >= 0) viewTrackPart(xmax-1,zmin, camx-1,camz+1, -1,1, camy); //voor+rechter strook
	}

	glPopMatrix();
}

void CTERenderer::viewTrackPart(
	int xmin,int ymin,
	int xmax,int ymax,
	int dx,  int dy,
	int cur_zpos)
{
	int lengte = m_Manager->getTrack()->m_L;
	int  breedte = m_Manager->getTrack()->m_W;

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

void CTERenderer::viewPilaar(int x, int y, int cur_zpos)
{
	int  breedte = m_Manager->getTrack()->m_W;
	int hoogte = m_Manager->getTrack()->m_H;

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

		//One LOD closer in the track editor:
		if(lod>1) lod--;

		int pilaar_index = hoogte * y + hoogte * breedte * x;
		int ynu = 0;
		int rnu = 0;

		for (int i = 0; i < hoogte; i++) //bottom to top
		{
			STile temp = m_Manager->getTrack()->m_Track[pilaar_index + i]; //welke tile?

			if(temp.m_Model == 0) break; //0 = empty tile

			//goede hoogte
			int ystrax = temp.m_Z;

			if(ystrax >= camy)
			{
				for(int j = hoogte-1; j >= i; j--) //top to bottom
				{
					temp = m_Manager->getTrack()->m_Track[pilaar_index + j]; //welke tile?

					if(temp.m_Model > 0) //0 = empty tile
					{
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

						//draw the model
						m_Manager->getTile(temp.m_Model)->draw(&m_Settings, NULL, lod, 0.0);
					}
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
			m_Manager->getTile(temp.m_Model)->draw(&m_Settings, NULL, lod, 0.0);
		}

	glPopMatrix();
}
