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
#include "console.h"
#include "timer.h"

//TODO: remove this when not debugging
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
		CVector fc = m_GraphicWorld->m_Background->getColor();
		m_FogColor[0] = fc.x;
		m_FogColor[1] = fc.y;
		m_FogColor[2] = fc.z;
		m_FogColor[3] = 1.0;
		glClearColor(m_FogColor[0],m_FogColor[1],m_FogColor[2],m_FogColor[3]);

		if(m_Settings.m_FogMode >= 0)
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
	//TODO
}

void CGameRenderer::setCameras(CGameCamera **cams, unsigned int num)
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

			//3D part
			selectCamera(i);
			renderScene();

			//2D part
			selectCamera(i, false);
			viewDashboard(i);
			//fprintf(stderr, "Viewport output: %.5f\n\n\n", _DebugTimer.getTime() - tcam);
		}

		//fprintf(stderr, "Rendering: %.5f\n\n\n", _DebugTimer.getTime() - tstart);
	}
}

void CGameRenderer::clearScreen()
{
	if(m_Settings.m_ZBuffer)
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

	if(currentSide == 0 || m_Settings.m_UpdRefAllSides)
	{
		currentObject++;
		if(currentObject >= (int)(m_NumCameras * m_World->getNumObjects(CDataObject::eMovingObject))) currentObject = 0;
	}

	//initialising reflections if they don't exist
	unsigned int numRequired = m_NumCameras * m_World->getNumObjects(CDataObject::eMovingObject);
	unsigned int numPresent = m_MovingObjectReflections.size();
	if(numPresent < numRequired)
		for(unsigned int i=0; i< (numRequired - numPresent); i++)
		{
			CDynamicReflection r(m_Settings.m_TexSmooth, m_Settings.m_ReflectionSize);
			m_MovingObjectReflections.push_back(r);
		}

	//Updating the reflection images
	if(m_Settings.m_UpdRefAllObjs)
	{
		for(unsigned int cam=0; cam < m_NumCameras; cam++)
		{
			m_CurrentCamera = cam;

			for(unsigned int obj=0; obj < m_World->getNumObjects(CDataObject::eMovingObject); obj++)
			{
				CMovingObject *mo = theWorld->getMovingObject(obj);
				CVector pos = mo->m_Position;

				//we don't have to update a reflection that is not visible
				if((pos - m_Cameras[cam]->getPosition()).abs() > m_Settings.m_ReflectionDist) continue;

				//camera
				CCamera front;
				pos += mo->m_OrientationMatrix * mo->getCameraPos();
				front.setPosition(pos);
				front.setOrientation(m_Cameras[cam]->getOrientation());

				//update reflection
				m_UpdateBodyReflection = obj;
				unsigned int index = cam + m_NumCameras * obj;

				if(m_Settings.m_UpdRefAllSides)
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

		CMovingObject *mo = theWorld->getMovingObject(obj);
		CVector pos = mo->m_Position;

		//we don't have to update a reflection that is not visible
		if((pos - m_Cameras[cam]->getPosition()).abs() <= m_Settings.m_ReflectionDist)
		{
			//camera
			CCamera front;
			pos += mo->m_OrientationMatrix * mo->getCameraPos();
			front.setPosition(pos);
			front.setOrientation(m_Cameras[cam]->getOrientation());

			//update reflection
			m_UpdateBodyReflection = obj;
			unsigned int index = cam + m_NumCameras * obj;

			if(m_Settings.m_UpdRefAllSides)
				{m_MovingObjectReflections[index].update(this, &front);}
			else
				{m_MovingObjectReflections[index].update(this, &front, currentSide);}
		}
	}

	m_UpdateBodyReflection = -1;
}

void CGameRenderer::selectCamera(unsigned int n, bool threed)
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
	GLfloat far = TILESIZE * m_Settings.m_VisibleTiles;
	float hor_mul = near / 5.0;
	GLfloat xs = ratio*hor_mul;
	GLfloat ys = 1.0*hor_mul;

	glViewport( x, y, w, h );
	m_ViewportW = w;
	m_ViewportH = h;

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	if(threed)
	{
		glFrustum( -xs, xs, -ys, ys, near, far );
	}
	else
	{
		glOrtho(0, w, 0, h, -1, 1);
	}

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

	if(m_Settings.m_UseBackground)
	{
		viewBackground();
		if(m_Settings.m_ZBuffer)
			glClear( GL_DEPTH_BUFFER_BIT );
	}

	//fprintf(stderr, "renderTrack start: %.3f\n", _DebugTimer.getTime());

	//Lighting:
	CTrack *theTrack = theWorld->getTrack();
	CVector lightDir = theTrack->m_LightDirection;
	CVector lightCol = theTrack->m_LightColor;
	CVector ambCol = theTrack->m_AmbientColor;
	
	GLfloat light_color[] = {lightCol.x, lightCol.y, lightCol.z, 1.0};
	GLfloat light_direction[] = {-lightDir.x, -lightDir.y, -lightDir.z, 0.0};
	GLfloat ambient_color[] = {ambCol.x, ambCol.y, ambCol.z, 1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, light_direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_color);

	const CVector &camera = m_Camera->getPosition();

	glTranslatef (-camera.x, -camera.y, -camera.z);

	camx = (int)(0.5 + (camera.x)/TILESIZE);
	camy = (int)(0.5 + (camera.y)/VERTSIZE);
	camz = (int)(0.5 + (camera.z)/TILESIZE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//Draw the track
	if(m_Settings.m_TrackDisplayList)
		{viewTrack_displaylist();}
	else
		{viewTrack_normal();}

	if(!m_Settings.m_ReflectionDrawMovingObjects && m_UpdateBodyReflection >= 0)
		return; //don't draw moving objects

	//Draw the moving objects
	//float tobj = _DebugTimer.getTime();
	int num_objs = m_World->getNumObjects(CDataObject::eMovingObject);
	for(int i=0; i<num_objs; i++)
		if(i != m_UpdateBodyReflection) //don't draw the body in its own reflection
			viewMovObj(i);
	//fprintf(stderr, "Drawing moving objects: %.5f\n", _DebugTimer.getTime() - tobj);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void CGameRenderer::viewBackground()
{
	//if(m_FogMode != -1)
	//	glDisable(GL_FOG);
	if(m_Settings.m_ZBuffer)
		glDisable(GL_DEPTH_TEST);

	m_GraphicWorld->m_Background->draw();

	//if(m_FogMode != -1)
	//	glEnable(GL_FOG);
	if(m_Settings.m_ZBuffer)
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
	int lengte = m_World->getTrack()->m_L;
	int  breedte = m_World->getTrack()->m_W;

	//printf ("x,y,z = %d,%d,%d\n",camx,camy,camz);

	//Nu: de dynamische begrenzing om weergavesnelheid te vergroten
	int xmin = (camx-m_Settings.m_VisibleTiles < 0)? 0 : camx-m_Settings.m_VisibleTiles;
	int xmax = (camx+m_Settings.m_VisibleTiles > lengte)? lengte : camx+m_Settings.m_VisibleTiles;
	int zmin = (camz-m_Settings.m_VisibleTiles < 0)? 0 : camz-m_Settings.m_VisibleTiles;
	int zmax = (camz+m_Settings.m_VisibleTiles > breedte)? breedte : camz+m_Settings.m_VisibleTiles;

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
	int lengte = m_World->getTrack()->m_L;
	int  breedte = m_World->getTrack()->m_W;

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
	int  breedte = m_World->getTrack()->m_W;
	int hoogte = m_World->getTrack()->m_H;

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
			STile temp = m_World->getTrack()->m_Track[pilaar_index + i]; //welke tile?

			if(temp.m_Model == 0) break; //0 = empty tile

			//goede hoogte
			int ystrax = temp.m_Z;

			if(ystrax >= camy)
			{
				for(int j = hoogte-1; j >= i; j--) //top to bottom
				{
					temp = m_World->getTrack()->m_Track[pilaar_index + j]; //welke tile?

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
						m_GraphicWorld->getTile(temp.m_Model)->draw(&m_Settings, m_GraphicWorld->m_EnvMap, lod);
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
			m_GraphicWorld->getTile(temp.m_Model)->draw(&m_Settings, m_GraphicWorld->m_EnvMap, lod);
		}

	glPopMatrix();
}

void CGameRenderer::viewMovObj(unsigned int n)
{
	CMovingObject *mo = theWorld->getMovingObject(n);

	for(unsigned int i=mo->m_Bodies.size(); i > 0; i--) //TODO: depth sorting?
	{
		CBody &b = mo->m_Bodies[i-1];
		CVector r = b.m_Position;

		float dist = (m_Camera->getPosition() - r).abs();
		if(dist > TILESIZE * m_Settings.m_VisibleTiles) continue; //not visible

		int lod;
		if(dist > TILESIZE * 7)
			lod = 4;
		else if(dist > TILESIZE * 3)
			lod = 3;
		else if(dist > TILESIZE * 1)
			lod = 2;
		else
			lod = 1;

		lod += m_Settings.m_MovingObjectLOD;
		if(lod > 4) lod = 4;
		if(lod < 1) lod = 1;

		glPushMatrix();
		glTranslatef (r.x, r.y, r.z);
		glMultMatrixf(b.m_OrientationMatrix.gl_mtr());

		//The model

		//The reflection
		if(dist < m_Settings.m_ReflectionDist)
		{
			m_GraphicWorld->getMovObjBound(b.m_Body)->draw(
				&m_Settings,
				&m_MovingObjectReflections[m_CurrentCamera + m_NumCameras * n],
				lod);
		}
		else
		{
			m_GraphicWorld->getMovObjBound(b.m_Body)->draw(&m_Settings, NULL, lod);
		}

		glPopMatrix();
	}
}

void CGameRenderer::viewDashboard(unsigned int n)
{
	//The object:
	CMovingObject *theObj = theWorld->getMovingObject( ((CGameCamera *)(m_Cameras[n]))->m_PrimaryTarget );

	int w = m_ViewportW;
	int h = m_ViewportH;

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	if(m_Settings.m_ZBuffer) glDisable(GL_DEPTH_TEST);
	if(m_Settings.m_FogMode >= 0) glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);


	theConsoleFont->enable();

	//The message in the middle:
	CString message;
	float msgAlpha = 1.0;
	if(theObj->m_IncomingMessages.size() > 0)
	{
		//remove all messages but the last:
		if(theObj->m_IncomingMessages.size() > 1)
		{
			theObj->m_IncomingMessages[0] = theObj->m_IncomingMessages.back();
			theObj->m_IncomingMessages.resize(1); //delete all messages except the last
		}

		float racingTime = CTimer().getTime() - theWorld->m_GameStartTime;
		float msgAge = racingTime - theObj->m_IncomingMessages[0].m_SendTime;
		float maxAge = 1.0 + 0.1 * theObj->m_IncomingMessages[0].m_Message.length();
		float fadeAge = 1.0;

		/*
		printf("msg \"%s\" age = %.2f - %.2f = %.2f\n",
			theObj->m_IncomingMessages[0].m_Message.c_str(),
			racingTime, theObj->m_IncomingMessages[0].m_SendTime, msgAge);
		*/

		if(msgAge < maxAge)
		{
			message = theObj->m_IncomingMessages[0].m_Message;

			msgAge = msgAge - maxAge + fadeAge;
			if(msgAge > 0.0)
			{
				msgAlpha = 1.0 - msgAge / fadeAge;
			}
		}
		else
		{
			theObj->m_IncomingMessages.clear(); //also delete the last message
		}
	}

	if(message.length() > 0)
	{
		float size = 2.0; //character size
		glPushMatrix();
		glTranslatef(
			0.5*(w - message.length() * size * theConsoleFont->getFontW()),
			0.5*(h + theConsoleFont->getFontH()),
			0);
		glScalef(size,size,size);
		glColor4f(1,1,1,msgAlpha);
		theConsoleFont->drawString(message);
		glColor4f(1,1,1,1);
		glPopMatrix();
	}

	theConsoleFont->disable();
	
	glEnable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	if(m_Settings.m_ZBuffer) glEnable(GL_DEPTH_TEST);
	if(m_Settings.m_FogMode >= 0) glEnable(GL_FOG);
}
