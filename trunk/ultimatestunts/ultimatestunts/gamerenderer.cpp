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

//TODO: remove this when not debugging
#include "timer.h"
CTimer _DebugTimer;

CGameRenderer::CGameRenderer(const CWinSystem *winsys) : CRenderer(winsys)
{
	m_World = theWorld;
	m_GraphicWorld = new CGraphicWorld();
	m_NumCameras = 1;

	m_UpdateBodyReflection = -1;
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
		CVector fc = m_GraphicWorld->m_Background.getColor();
		m_FogColor[0] = fc.x;
		m_FogColor[1] = fc.y;
		m_FogColor[2] = fc.z;
		m_FogColor[3] = 1.0;
		glClearColor(m_FogColor[0],m_FogColor[1],m_FogColor[2],m_FogColor[3]);

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

void CGameRenderer::setCameras(CCamera **cams, unsigned int num)
{
	m_Cameras = cams;
	m_NumCameras = num;
}

void CGameRenderer::update()
{
	if(m_UpdateBodyReflection >= 0)
	{
		clearScreen();
		renderScene();
	}
	else
	{
		//float tstart = _DebugTimer.getTime();
		
		updateReflections();
		//fprintf(stderr, "Update reflections: %.5f\n\n\n", _DebugTimer.getTime() - tstart);

		clearScreen();
		for(unsigned int i=0; i < m_NumCameras; i++)
		{
			//float tcam = _DebugTimer.getTime();
			m_CurrentCamera = i;
			selectCamera(i);
			renderScene();
			//fprintf(stderr, "Viewport output: %.5f\n\n\n", _DebugTimer.getTime() - tcam);
		}

		//fprintf(stderr, "Rendering: %.5f\n\n\n", _DebugTimer.getTime() - tstart);
	}
}

void CGameRenderer::clearScreen()
{
	if(m_ZBuffer)
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	else
		glClear( GL_COLOR_BUFFER_BIT );
}

void CGameRenderer::updateReflections()
{
	static int currentSide = 0;
	static int currentObject = 0;
	currentSide++;
	if(currentSide >= 6) currentSide = 0;

	if(currentSide == 0 || m_UpdRefAllSides)
	{
		currentObject++;
		if(currentObject >= (int)(m_NumCameras * m_World->m_MovObjs.size())) currentObject = 0;
	}

	//initialising reflections if they don't exist
	unsigned int numRequired = m_NumCameras * m_World->m_MovObjs.size();
	unsigned int numPresent = m_MovingObjectReflections.size();
	if(numPresent < numRequired)
		for(unsigned int i=0; i< (numRequired - numPresent); i++)
		{
			CReflection r(m_TexSmooth, m_ReflectionSize);
			m_MovingObjectReflections.push_back(r);
		}

	//Updating the reflection images
	if(m_UpdRefAllObjs)
	{
		for(unsigned int cam=0; cam < m_NumCameras; cam++)
		{
			m_CurrentCamera = cam;

			for(unsigned int obj=0; obj < m_World->m_MovObjs.size(); obj++)
			{
				CMovingObject *mo = theWorld->m_MovObjs[obj];
				CVector pos = mo->m_Bodies[0].getPosition();

				//we don't have to update a reflection that is not visible
				if((pos - m_Cameras[cam]->getPosition()).abs() > m_ReflectionDist) continue;

				//camera
				CCamera front;
				pos += mo->m_Bodies[0].getOrientationMatrix() * mo->getCameraPos();
				front.setPosition(pos);
				front.setOrientation(m_Cameras[cam]->getOrientation());

				//update reflection
				m_UpdateBodyReflection = obj;
				unsigned int index = cam + m_NumCameras * obj;

				if(m_UpdRefAllSides)
					{m_MovingObjectReflections[index].update(this, &front);}
				else
					{m_MovingObjectReflections[index].update(this, &front, currentSide);}
			}

		}
	}
	else
	{
		unsigned int obj = currentObject / m_NumCameras;
		unsigned int cam = currentObject - m_NumCameras * obj;
		m_CurrentCamera = cam;

		CMovingObject *mo = theWorld->m_MovObjs[obj];
		CVector pos = mo->m_Bodies[0].getPosition();

		//we don't have to update a reflection that is not visible
		if((pos - m_Cameras[cam]->getPosition()).abs() <= m_ReflectionDist)
		{
			//camera
			CCamera front;
			pos += mo->m_Bodies[0].getOrientationMatrix() * mo->getCameraPos();
			front.setPosition(pos);
			front.setOrientation(m_Cameras[cam]->getOrientation());

			//update reflection
			m_UpdateBodyReflection = obj;
			unsigned int index = cam + m_NumCameras * obj;

			if(m_UpdRefAllSides)
				{m_MovingObjectReflections[index].update(this, &front);}
			else
				{m_MovingObjectReflections[index].update(this, &front, currentSide);}
		}
	}

	m_UpdateBodyReflection = -1;
}

void CGameRenderer::selectCamera(unsigned int n)
{
	unsigned int sw = m_WinSys->getWidth();
	unsigned int sh = m_WinSys->getHeight();

	//default for 1 camera:
	unsigned int x = 0, y = 0, w = sw, h = sh;

	if(m_NumCameras == 2)
	{
		if(n == 0) y = sh / 2;
		h = sh / 2;
	}
	if(m_NumCameras > 2)
	{
		if(n == 0 || n == 1) y = sh / 2;
		if(n == 1 || n == 3) x = sw / 2;
		
		w = sw / 2;
		h = sh / 2;
	}
	
	//Set up openGL viewport + frustum stuff to window size

	float ratio = (float) w / (float) h;
	GLfloat near = 1.0;
	GLfloat far = TILESIZE * m_VisibleTiles;
	float hor_mul = near / 5.0;
	GLfloat xs = ratio*hor_mul;
	GLfloat ys = 1.0*hor_mul;

	glViewport( x, y, w, h );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glFrustum( -xs, xs, -ys, ys, near, far );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	setCamera(m_Cameras[n]);
}

void CGameRenderer::renderScene()
{
	//fprintf(stderr, "renderBackground start: %.3f\n", _DebugTimer.getTime());

	const CMatrix &cammat = m_Camera->getOrientation();
	//glLoadIdentity();
	glLoadMatrixf(cammat.transpose().gl_mtr());

	if(m_UseBackground)
	{
		viewBackground();
		if(m_ZBuffer)
			glClear( GL_DEPTH_BUFFER_BIT );
	}

	//fprintf(stderr, "renderTrack start: %.3f\n", _DebugTimer.getTime());

	//Lighting:
	GLfloat sun_diffuse[] = {1.0, 1.0, 0.8, 1.0};  /* Yellow 'sun' diffuse light. */
	GLfloat sun_position[] = {0.69, 0.7, 0.2, 0.0};  /* Infinite light location. */

	glLightfv(GL_LIGHT0, GL_POSITION, sun_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_diffuse);

	const CVector &camera = m_Camera->getPosition();

	glTranslatef (-camera.x, -camera.y, -camera.z);

	camx = (int)(0.5 + (camera.x)/TILESIZE);
	camy = (int)(0.5 + (camera.y)/VERTSIZE);
	camz = (int)(0.5 + (camera.z)/TILESIZE);

	viewTrack_displaylist();

	if(!m_ReflectionDrawMovingObjects && m_UpdateBodyReflection >= 0)
		return; //don't draw moving objects

	//Tijdelijke plaats om auto's te renderen: achteraan
	//float tobj = _DebugTimer.getTime();
	int num_objs = m_World->m_MovObjs.size();
	for(int i=0; i<num_objs; i++)
		if(i != m_UpdateBodyReflection) //don't draw the body in its own reflection
			viewMovObj(i);
	//fprintf(stderr, "Drawing moving objects: %.5f\n", _DebugTimer.getTime() - tobj);
}

void CGameRenderer::viewBackground()
{
	//if(m_FogMode != -1)
	//	glDisable(GL_FOG);
	if(m_ZBuffer)
		glDisable(GL_DEPTH_TEST);

	m_GraphicWorld->m_Background.draw();

	//if(m_FogMode != -1)
	//	glEnable(GL_FOG);
	if(m_ZBuffer)
		glEnable(GL_DEPTH_TEST);
}

void CGameRenderer::viewTrack_displaylist()
{
	static vector<int> x;
	static vector<int> y;
	static vector<int> z;
	static vector<unsigned int> dispList;

	while(m_CurrentCamera >= x.size())
	{
		//generate new camera info
		x.push_back(-1);
		y.push_back(-1);
		z.push_back(-1);
		dispList.push_back(glGenLists(1));
	}

	unsigned int cam = m_CurrentCamera;
	//fprintf(stderr, "Camera %d\n", cam);

	if(camx != x[cam] || camy != y[cam] || camz != z[cam]) //the display list should be updated
	{
		x[cam] = camx;
		y[cam] = camy;
		z[cam] = camz;
		//float tstart = _DebugTimer.getTime();
		glNewList(dispList[cam], GL_COMPILE);
		viewTrack_normal();
		glEndList();
		//fprintf(stderr, "Updating dispList: %.5f\n", _DebugTimer.getTime() - tstart);
	}

	//float tstart = _DebugTimer.getTime();
	glCallList(dispList[cam]);
	//fprintf(stderr, "Calling dispList: %.5f\n", _DebugTimer.getTime() - tstart);

	//TODO: find some way to delete unused lists
}

void CGameRenderer::viewTrack_normal()
{
	glPushMatrix();

	//Nu volgt de weergave-routine
	int lengte = m_World->m_L;
	int  breedte = m_World->m_W;

	//printf ("x,y,z = %d,%d,%d\n",camx,camy,camz);

	//Nu: de dynamische begrenzing om weergavesnelheid te vergroten
	int xmin = (camx-m_VisibleTiles < 0)? 0 : camx-m_VisibleTiles;
	int xmax = (camx+m_VisibleTiles > lengte)? lengte : camx+m_VisibleTiles;
	int zmin = (camz-m_VisibleTiles < 0)? 0 : camz-m_VisibleTiles;
	int zmax = (camz+m_VisibleTiles > breedte)? breedte : camz+m_VisibleTiles;

	//if(!m_ZBuffer)
	//always back to front (because of transparency)
	{
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

	}
	/*
	else //zbuffer
	{
		//printf("Using z-buffering\n");
		viewTrackPart(xmin, zmin, xmax, zmax, 1, 1, camy);
	}
	*/

	glPopMatrix();
}

void CGameRenderer::viewTrackPart(
	int xmin,int ymin,
	int xmax,int ymax,
	int dx,  int dy,
	int cur_zpos)
{
	int lengte = m_World->m_L;
	int  breedte = m_World->m_W;

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
	int  breedte = m_World->m_W;
	int hoogte = m_World->m_H;

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

		for (int i = 0; i < hoogte; i++) //bottom to top
		{
			CTile temp = m_World->m_Track[pilaar_index + i]; //welke tile?

			if(temp.m_Model == 0) break; //0 = empty tile

			//goede hoogte
			int ystrax = temp.m_Z;

			if(ystrax >= camy)
			{
				for(int j = hoogte-1; j >= i; j--) //top to bottom
				{
					temp = m_World->m_Track[pilaar_index + j]; //welke tile?

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

						//tekenen
						m_GraphicWorld->m_Tiles[temp.m_Model].draw(lod);
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
			m_GraphicWorld->m_Tiles[temp.m_Model].draw(lod);
		}

	glPopMatrix();
}

void CGameRenderer::viewMovObj(unsigned int n)
{
	CMovingObject *mo = theWorld->m_MovObjs[n];

	for(unsigned int i=mo->m_Bodies.size(); i > 0; i--) //TODO: depth sorting?
	{
		CBody &b = mo->m_Bodies[i-1];
		CVector r = b.getPosition();

		float dist = (m_Camera->getPosition() - r).abs();
		if(dist > TILESIZE * m_VisibleTiles) continue; //not visible

		int lod;
		if(dist > TILESIZE * 7)
			lod = 4;
		else if(dist > TILESIZE * 3)
			lod = 3;
		else if(dist > TILESIZE * 1)
			lod = 2;
		else
			lod = 1;

		lod += m_MovingObjectLOD;
		if(lod > 4) lod = 4;
		if(lod < 1) lod = 1;

		glPushMatrix();
		glTranslatef (r.x, r.y, r.z);
		glMultMatrixf(b.getOrientationMatrix().gl_mtr());

		//The model
		m_GraphicWorld->m_MovingObjects[b.m_Body].draw(lod);

		//The reflection
		if(dist < m_ReflectionDist)
		{
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			GLfloat oldambient[4];
			GLfloat newambient[] = {1.0,1.0,1.0,1.0};
			glGetFloatv(GL_LIGHT_MODEL_AMBIENT, oldambient);
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, newambient);

			glPolygonOffset(0.0, -1.0);
			//m_GraphicWorld->m_EnvMap.draw(1);
			m_MovingObjectReflections[m_CurrentCamera + m_NumCameras * n].draw();
			m_GraphicWorld->m_MovingObjects[b.m_Body].draw(0);

			glDisable(GL_POLYGON_OFFSET_FILL);
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, oldambient);
		}

		glPopMatrix();
	}
}
