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
#include <cmath>
#include "pi.h"

#include "gamerenderer.h"
#include "console.h"
#include "timer.h"

#define FOV_MULTIPLIER (0.25)

//TODO: remove this when not debugging
CTimer _DebugTimer;

CGameRenderer::CGameRenderer(const CWinSystem *winsys) : CRenderer(winsys)
{
	m_GraphicWorld = new CGraphicWorld();
	m_NumCameras = 1;

	m_UpdateBodyReflection = -1;
}

bool CGameRenderer::reloadConfiguration()
{
	if(!CRenderer::reloadConfiguration()) return false;

	if(!m_GraphicWorld->reloadConfiguration()) return false;

	return true;
}

CGameRenderer::~CGameRenderer()
{
	unloadTrackData();
	delete m_GraphicWorld;
}

bool CGameRenderer::loadTrackData()
{
	if(!m_GraphicWorld->loadWorld())
		return false;

	CVector fc = m_GraphicWorld->m_Background->getColor();
	m_FogColor[0] = fc.x;
	m_FogColor[1] = fc.y;
	m_FogColor[2] = fc.z;
	m_FogColor[3] = 1.0;
	glClearColor(m_FogColor[0],m_FogColor[1],m_FogColor[2],m_FogColor[3]);

	if(m_Settings.m_FogMode >= 0)
		glFogfv(GL_FOG_COLOR, m_FogColor);

	//Lighting:
	CTrack *theTrack = theWorld->getTrack();
	CVector lightCol = theTrack->m_LightColor;
	CVector ambCol = theTrack->m_AmbientColor;

	GLfloat light_color[] = {lightCol.x, lightCol.y, lightCol.z, 1.0};
	GLfloat specular_color[] = {3.0*lightCol.x, 3.0*lightCol.y, 3.0*lightCol.z, 1.0};
	GLfloat ambient_color[] = {ambCol.x, ambCol.y, ambCol.z, 1.0};

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular_color);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient_color);

	return true;
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

		updateShadows();
		
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
			viewDashboard(m_Cameras[i]->getTrackedObject());
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

void CGameRenderer::updateShadows()
{
	if(m_Settings.m_ShadowSize <= 4) return;

	CVector lightDir     = theWorld->getTrack()->m_LightDirection;
	CVector lightColor   = theWorld->getTrack()->m_LightColor;
	CVector ambientColor = theWorld->getTrack()->m_AmbientColor;

	CVector totalColor = 1.4*lightColor + ambientColor;

	CVector shadowColor(
		lightColor.x / totalColor.x,
		lightColor.y / totalColor.y,
		lightColor.z / totalColor.z);

	for(unsigned int i=0; i < m_GraphicWorld->getNumObjects(CDataObject::eMovingObject); i++)
	{
		CDynamicShadow *shadow = m_GraphicWorld->getMovObjShadow(i);
		shadow->setLightSource(lightDir, shadowColor);
		shadow->update(&m_Settings, theWorld->m_LastTime);
	}
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
		if(currentObject >= (int)(m_NumCameras * theWorld->getNumObjects(CDataObject::eMovingObject)))
			currentObject = 0;
	}

	/*
	//initialising reflections if they don't exist
	unsigned int numRequired = m_NumCameras * m_World->getNumObjects(CDataObject::eMovingObject);
	unsigned int numPresent = m_MovingObjectReflections.size();
	if(numPresent < numRequired)
		for(unsigned int i=0; i< (numRequired - numPresent); i++)
		{
			CDynamicReflection r(m_Settings.m_ReflectionSize);
			m_MovingObjectReflections.push_back(r);
		}
	*/

	//Updating the reflection images
	if(m_Settings.m_UpdRefAllObjs)
	{
		for(unsigned int cam=0; cam < m_NumCameras; cam++)
		{
			m_CurrentCamera = cam;

			for(unsigned int obj=0; obj < theWorld->getNumObjects(CDataObject::eMovingObject); obj++)
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

				CDynamicReflection *theRefl = m_GraphicWorld->getMovObjReflection(obj, cam);
				//unsigned int index = cam + m_NumCameras * obj;
				//CDynamicReflection *theRefl = m_MovingObjectReflections[index];

				if(m_Settings.m_UpdRefAllSides)
					{theRefl->update(this, &front);}
				else
					{theRefl->update(this, &front, currentSide);}
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

			CDynamicReflection *theRefl = m_GraphicWorld->getMovObjReflection(obj, cam);
			//unsigned int index = cam + m_NumCameras * obj;
			//CDynamicReflection *theRefl = m_MovingObjectReflections[index];

			if(m_Settings.m_UpdRefAllSides)
				{theRefl->update(this, &front);}
			else
				{theRefl->update(this, &front, currentSide);}
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
	float hor_mul = near * FOV_MULTIPLIER;
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
	CVector lightDir = theWorld->getTrack()->m_LightDirection;
	GLfloat light_direction[] = {-lightDir.x, -lightDir.y, -lightDir.z, 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_direction);

	const CVector &camera = m_Camera->getPosition();

	glTranslatef (-camera.x, -camera.y, -camera.z);

	//Camera position in tile units
	camx = (int)(0.5 + (camera.x)/TILESIZE);
	camy = (int)(0.5 + (camera.y)/VERTSIZE);
	camz = (int)(0.5 + (camera.z)/TILESIZE);

	//Camera clipping plane
	m_CamPlaneNor  = cammat * CVector(0.0,0.0,-1.0);
	m_CamPlaneDist = camera.dotProduct(m_CamPlaneNor);

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
	CGameCamera *gcam = (CGameCamera *)m_Camera;
	int num_objs = theWorld->getNumObjects(CDataObject::eMovingObject);
	for(int i=0; i<num_objs; i++)
	{
		if(i == m_UpdateBodyReflection)
			continue; //don't draw the object in its own reflection
		if(i == gcam->getTrackedObject() && gcam->getCameraMode() == CGameCamera::In)
			continue; //don't draw the object in "In object" camera view
		viewMovObj(i);
	}
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
	int lengte = theWorld->getTrack()->m_L;
	int  breedte = theWorld->getTrack()->m_W;

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
	int lengte = theWorld->getTrack()->m_L;
	int  breedte = theWorld->getTrack()->m_W;

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
	int  breedte = theWorld->getTrack()->m_W;
	int hoogte = theWorld->getTrack()->m_H;

	CVector tilepos;


	glPushMatrix();

		int lod;
		int dx=abs(camx-x);
		int dy=abs(camz-y); //This (camz) is not a typo!!!

		int d = dx;
		if(dy>d) d=dy; //d is the largest of the 2

		if(d>7)
			lod = 4;
		else if(d>3)
			lod = 3;
		else if(d>1)
			lod = 2;
		else
			lod = 1;

		float dist = TILESIZE*sqrt((double)(dx*dx+dy*dy));
		CReflection *refl = NULL;
		if(dist < m_Settings.m_ReflectionDist)
			refl = m_GraphicWorld->m_EnvMap;

		int pilaar_index = hoogte * y + hoogte * breedte * x;
		int ynu = 0;
		int rnu = 0;

		for (int i = 0; i < hoogte; i++) //bottom to top
		{
			STile temp = theWorld->getTrack()->m_Track[pilaar_index + i]; //which tile?

			if(temp.m_Model == 0) break; //0 = empty tile

			//right height
			int ystrax = temp.m_Z;

			tilepos = CVector(x*TILESIZE, ystrax*VERTSIZE, y*TILESIZE);
			if(tilepos.dotProduct(m_CamPlaneNor) < m_CamPlaneDist-TILESIZE)
				continue; //behind clipping plane

			if(ystrax >= camy)
			{
				for(int j = hoogte-1; j >= i; j--) //top to bottom
				{
					temp = theWorld->getTrack()->m_Track[pilaar_index + j]; //which tile?

					if(temp.m_Model > 0) //0 = empty tile
					{
						//right height
						ystrax = temp.m_Z;

						tilepos = CVector(x*TILESIZE, ystrax*VERTSIZE, y*TILESIZE);
						if(tilepos.dotProduct(m_CamPlaneNor) < m_CamPlaneDist-TILESIZE)
							continue; //behind clipping plane

						glTranslatef(0, VERTSIZE*(ystrax-ynu),0);
						ynu = ystrax;

						//right orientatie
						int rstrax = temp.m_R;
						if (rstrax != rnu)
						{
							glRotatef(90*(rstrax-rnu),0,1,0);
							rnu = rstrax;
						}

						//draw the model
						m_GraphicWorld->getTile(temp.m_Model)->draw(
							&m_Settings, refl, lod, theWorld->m_LastTime);
					}
				}
				break;
			}

			glTranslatef(0, VERTSIZE*(ystrax-ynu),0);
			ynu = ystrax;

			//right orientatie
			int rstrax = temp.m_R;
			if (rstrax != rnu)
			{
				glRotatef(90*(rstrax-rnu),0,1,0);
				rnu = rstrax;
			}

			//draw
			m_GraphicWorld->getTile(temp.m_Model)->draw(
				&m_Settings, refl, lod, theWorld->m_LastTime);
		}

	glPopMatrix();
}

void CGameRenderer::viewMovObj(unsigned int n)
{
	CMovingObject *mo = theWorld->getMovingObject(n);

	//Determine lighting
	//TODO: cache the results of this for split screen
	bool inShadow = false;
	{
		CVector lightdir = -(theWorld->getTrack()->m_LightDirection);

		//Check whether we are in the shadow of something
		CCollisionDetector &detector = theWorld->m_Detector;
		float colDist = detector.getLineCollision(mo->m_Position, lightdir);
		if(colDist > 0.0) inShadow = true;
	}

	if(inShadow) glDisable(GL_LIGHT0);

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
			CDynamicReflection *theRefl = m_GraphicWorld->getMovObjReflection(n, m_CurrentCamera);
			//CDynamicReflection *theRefl = m_MovingObjectReflections[m_CurrentCamera + m_NumCameras * n];

			m_GraphicWorld->getMovObjBound(b.m_Body)->draw(
				&m_Settings, theRefl, lod, theWorld->m_LastTime);
		}
		else
		{
			m_GraphicWorld->getMovObjBound(b.m_Body)->draw(
				&m_Settings, NULL, lod, theWorld->m_LastTime);
		}

		glPopMatrix();
	}

	if(inShadow) glEnable(GL_LIGHT0);


	//The shadow
	if(!inShadow && m_Settings.m_ShadowSize > 4)
	{
		CVector r = mo->m_Bodies[0].m_Position;
		const CCollisionFace *plane = theWorld->m_Detector.getGroundFace(r);
		if(plane != NULL)
		{
			CVector lightDir = theWorld->getTrack()->m_LightDirection.normal();
			float ldotn = lightDir.dotProduct(plane->nor);
			if(ldotn < -0.001) //plane is lighted
			{
				CVector difference = lightDir / ldotn;

				CDynamicShadow *shadow = m_GraphicWorld->getMovObjShadow(n);

				float ps = shadow->getPhysicalSize();
				CVector texcoords[] = {
					CVector(0,0,0),
					CVector(1,0,0),
					CVector(1,1,0),
					CVector(0,1,0)
					};
				CVector corners[4];
				for(unsigned int i=0; i<4; i++)
				{
					corners[i] = ps*(2*texcoords[i]-CVector(1,1,0));
					corners[i] *= shadow->getLightOrientation();
					corners[i] += r;
					corners[i] -= difference * (corners[i].dotProduct(plane->nor) - plane->d - 0.01);
				}

				shadow->enable();

				glBegin(GL_QUADS);
				for(unsigned int i=0; i<4; i++)
				{
					glTexCoord2f(texcoords[i].x,texcoords[i].y);
					glVertex3f(corners[i].x,corners[i].y,corners[i].z);
				}
				glEnd();

				shadow->disable();
			}
		}
	}
}

void CGameRenderer::viewDashboard(unsigned int n)
{
	if(m_Settings.m_ZBuffer) glDisable(GL_DEPTH_TEST);
	if(m_Settings.m_FogMode >= 0) glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	if( ((CGameCamera *)m_Camera)->getCameraMode() == CGameCamera::In )
	{
		m_GraphicWorld->getMovObjDashboard(n)->draw(m_ViewportW, m_ViewportH, CDashboard::eFull);
	}
	else
	{
		m_GraphicWorld->getMovObjDashboard(n)->draw(m_ViewportW, m_ViewportH, CDashboard::eGauges);
	}

	//Reflection debugging:
	/*
	glPushMatrix();
	glScalef(m_ViewportW, m_ViewportH, 1.0);
	glTranslatef(0.75, 0.75, 0.0);

	//glDisable(GL_BLEND);
	glColor4f(1,1,1,1);
	CDynamicReflection *theRefl = m_GraphicWorld->getMovObjReflection(0, 0);
	theRefl->enable(&m_Settings);
	theRefl->disable();

	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2f(-0.2,-0.2);
	glTexCoord2f(1,0);
	glVertex2f( 0.2,-0.2);
	glTexCoord2f(1,1);
	glVertex2f( 0.2, 0.2);
	glTexCoord2f(0,1);
	glVertex2f(-0.2, 0.2);
	glEnd();

	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //default blending function
	glPopMatrix();
	*/

	glLoadIdentity();
	viewLensFlare();

	glEnable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	if(m_Settings.m_ZBuffer) glEnable(GL_DEPTH_TEST);
	if(m_Settings.m_FogMode >= 0) glEnable(GL_FOG);
}

void CGameRenderer::viewLensFlare()
{
	//No lens flare if no images are specified
	if(m_GraphicWorld->m_LensFlare.size() == 0) return;

	CVector lightdir = -(theWorld->getTrack()->m_LightDirection);

	//Check whether we are in the shadow of something
	CCollisionDetector &detector = theWorld->m_Detector;
	float colDist = detector.getLineCollision(m_Camera->getPosition(), lightdir);
	if(colDist > 0.0) return; //collision: we are in a shadow

	CVector lightcol = theWorld->getTrack()->m_LightColor;
	const CMatrix &cammat = m_Camera->getOrientation();

	lightdir /= cammat;

	if(lightdir.z > -0.01) return; //behind camera
	lightdir.z = -lightdir.z;

	lightdir.x /= lightdir.z;
	lightdir.y /= lightdir.z;
	lightdir.z = 0.0;

	lightdir *= 0.5 / FOV_MULTIPLIER;
	lightdir.x *= float(m_ViewportH) / m_ViewportW;

	float distance = lightdir.abs();
	if(distance >= 2.0) return; //out of view
	float intensity = 0.5 * (2.0 - distance);

	float angle = atan2f(lightdir.y, lightdir.x) * (180.0/M_PI);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE); //additive blending

	glPushMatrix();
	glTranslatef(0.5*m_ViewportW, 0.5*m_ViewportH, 0.0);
	glScalef(m_ViewportW, m_ViewportW, 1.0);

	//Flare images
	for(unsigned int i=0; i < m_GraphicWorld->m_LensFlare.size(); i++)
	{
		const CGraphicWorld::SLensFlare &flare = m_GraphicWorld->m_LensFlare[i];

		CVector flarepos = flare.distance * lightdir;

		float flaresize = flare.size;
		float flareintensity = intensity;
		//if(i > 0) flareintensity *= 0.5; //Everything half the intensity of the sun

		glColor4f(lightcol.x, lightcol.y, lightcol.z, flareintensity);
		flare.image->draw();

		glPushMatrix();
		glTranslatef(flarepos.x, flarepos.y, 0.0);
		glRotatef(2 * angle, 0.0, 0.0, 1.0);

		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2f(-flaresize,-flaresize);
		glTexCoord2f(1,0);
		glVertex2f( flaresize,-flaresize);
		glTexCoord2f(1,1);
		glVertex2f( flaresize, flaresize);
		glTexCoord2f(0,1);
		glVertex2f(-flaresize, flaresize);
		glEnd();

		glPopMatrix();
	}

	//Whitening
	glColor4f(lightcol.x, lightcol.y, lightcol.z, 0.5*intensity);
	glBindTexture(GL_TEXTURE_2D, 0); //no texture
	glRectf(-0.5,-0.5,0.5,0.5);

	glPopMatrix();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //default blending function
	glColor4f(1,1,1,1);
}
