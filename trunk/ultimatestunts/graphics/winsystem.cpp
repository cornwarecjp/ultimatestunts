/***************************************************************************
                          winsystem.cpp  -  A window managing class
                             -------------------
    begin                : wo jan 15 2003
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

#include <stdio.h>
#include <stdlib.h>

#include "winsystem.h"

bool dummy_loopfunc()
{return false;} //exit immediately

CWinSystem::CWinSystem(const CLConfig &conf)
{
	//Default values:
	m_Flags = SDL_OPENGL|SDL_RESIZABLE;
	m_W = 640;
	m_H = 480;
	m_BPP = 24;
	m_VisibleTiles = 20;


	//Display variable:
	CString cnf = conf.getValue("graphics","display");
	printf("Display variable: \"%s\"\n", cnf.c_str());
	unsigned int pos = cnf.inStr(':');
	if(pos > 0 && pos < cnf.length()-1) //There is a ':' in cnf_display
	{
		CString s = cnf.mid(pos+1, cnf.length()-pos);
		cnf = cnf.mid(0, pos);
		pos = s.inStr('x');
		if(pos > 0 && pos < s.length()-1)
		{
			m_W = s.mid(0, pos).toInt();
			m_H = s.mid(pos+1, s.length()-pos).toInt();
			//TODO: support bpp setting
		}
	}

	if(cnf == "fullscreen")
		m_Flags = SDL_OPENGL|SDL_FULLSCREEN;


	//visible_tiles variable
	cnf = conf.getValue("graphics", "visible_tiles");
	if(cnf != "")
		m_VisibleTiles = cnf.toInt();

	//Some code coming from SDL gears
	SDL_Init(SDL_INIT_VIDEO);

	//SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	//SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	//SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	//SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	m_Screen = SDL_SetVideoMode(m_W, m_H, m_BPP, m_Flags);
	if ( ! m_Screen ) {
		fprintf(stderr, "Couldn't set %dx%d GL video mode: %s\n",
			m_W, m_W, SDL_GetError());
		SDL_Quit();
		exit(2);
	}
	SDL_WM_SetCaption("UltimateStunts", "ultimatestunts");

	//To set up openGL correctly, call reshape
	reshape(m_W, m_H);

	runLoop(dummy_loopfunc, true); //catch startup-events

	m_KeyState = SDL_GetKeyState(&m_NumKeys);
	m_WasPressed = new bool[m_NumKeys];
	for(int i=0; i<m_NumKeys; i++)
		m_WasPressed[i] = false;

}

CWinSystem::~CWinSystem()
{
	delete [] m_WasPressed;
	SDL_Quit();
}

int CWinSystem::runLoop( bool (CALLBACKFUN *loopfunc)(), bool swp)
{
	bool quit = false;

	//if loopfunc want to quit for some reason, then is returns false
	while(!quit)
	{
		SDL_Event event;

		while ( SDL_PollEvent(&event) )
		{
			switch(event.type)
			{
				case SDL_VIDEORESIZE:
					m_Screen = SDL_SetVideoMode(event.resize.w, event.resize.h, m_BPP,
						SDL_OPENGL|SDL_RESIZABLE);
					if ( m_Screen )
					{
						reshape(m_Screen->w, m_Screen->h);
					}
					else
					{
						/* Uh oh, we couldn't set the new video mode?? */
						fprintf(stderr, "Couldn't set %dx%d GL video mode: %s\n",
							event.resize.w, event.resize.h, SDL_GetError());
						SDL_Quit();
						exit(2);
					}
					break;
				case SDL_QUIT:
					quit = true;
					break;

				case SDL_KEYDOWN:
					m_WasPressed[event.key.keysym.sym] = true;
				case SDL_KEYUP:
					printf("Something has changed in the keystate\n");
					m_KeyState = SDL_GetKeyState(&m_NumKeys);
					break;
			}
		}

		quit = quit || !loopfunc();

		if(swp)
			SDL_GL_SwapBuffers();
	}

	return 0;
}

void CWinSystem::reshape(int w, int h)
{
	m_W = w; m_H = h;

	//Do some openGL stuff to adapt rendering to new window size

	float ratio = (float) w / (float) h;
	GLfloat near = 1.0;
	GLfloat far = TILESIZE * m_VisibleTiles;
	float hor_mul = near / 5.0;
	GLfloat xs = ratio*hor_mul;
	GLfloat ys = 1.0*hor_mul;

	glViewport( 0, 0, w, h );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glFrustum( -xs, xs, -ys, ys, near, far );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

bool CWinSystem::wasPressed(int c)
{
	bool ret = m_WasPressed[c];
	m_WasPressed[c] = false;
	return ret;
}
