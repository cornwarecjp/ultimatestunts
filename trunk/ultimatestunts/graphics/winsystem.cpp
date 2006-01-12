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

#include <cstdio>
#include <cstdlib>

#include "winsystem.h"

CWinSystem *theWinSystem = NULL;

bool dummy_loopfunc()
{return false;} //exit immediately

CWinSystem::CWinSystem(const CString &caption, const CLConfig &conf)
{
	theWinSystem = this;

	//Default values:
	m_Flags = SDL_OPENGL |SDL_RESIZABLE|SDL_ANYFORMAT;
	m_W = 640;
	m_H = 480;
	m_BPP = 24;
	m_VisibleTiles = 20;


	//Display variable:
	CString cnf = conf.getValue("graphics","display");
	printf("   Display variable: \"%s\"\n", cnf.c_str());
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
		m_Flags = SDL_OPENGL|SDL_FULLSCREEN|SDL_ANYFORMAT;


	//visible_tiles variable
	cnf = conf.getValue("graphics", "visible_tiles");
	if(cnf != "")
		m_VisibleTiles = cnf.toInt();

	//Some code coming from SDL gears
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER);

	//Minimum values:
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	printf("   Setting resolution to %dx%d:%d...\n", m_W, m_H, m_BPP);
	m_Screen = SDL_SetVideoMode(m_W, m_H, m_BPP, m_Flags);
	if ( ! m_Screen ) {
		fprintf(stderr, "   Couldn't set %dx%d GL video mode: %s\n",
			m_W, m_W, SDL_GetError());
		SDL_Quit();
		exit(2);
	}
	m_W = m_Screen->w;
	m_H = m_Screen->h;
	m_BPP = m_Screen->format->BitsPerPixel;
	printf("   ...Now working at %dx%d:%d\n", m_W, m_H, m_BPP);

	SDL_WM_SetCaption(caption.c_str(), "ultimatestunts");

	m_NumJoysticks = SDL_NumJoysticks();
	printf("   Found %d joysticks\n", m_NumJoysticks);

	if(m_NumJoysticks > 0)
	{
		SDL_JoystickEventState(SDL_ENABLE);
		m_Joystick = SDL_JoystickOpen(0);

		if(m_Joystick)
		{
			printf("   Opened Joystick 0\n");
			printf("   Name: %s\n", SDL_JoystickName(0));
			printf("   Number of Axes: %d\n", SDL_JoystickNumAxes(m_Joystick));
			printf("   Number of Buttons: %d\n", SDL_JoystickNumButtons(m_Joystick));
			printf("   Number of Balls: %d\n", SDL_JoystickNumBalls(m_Joystick));
		}
		else
		{
			printf("   Couldn't open Joystick 0\n");
			m_NumJoysticks = 0;
		}

	}

	runLoop(dummy_loopfunc, true); //catch startup-events

	m_KeyState = SDL_GetKeyState(&m_NumKeys);
	m_WasPressed = new bool[m_NumKeys];
	for(int i=0; i<m_NumKeys; i++)
		m_WasPressed[i] = false;

	if(m_NumJoysticks > 0)
		{m_NumJoyBtns = SDL_JoystickNumButtons(m_Joystick);}
	else
		{m_NumJoyBtns = 0;}
		

	m_JoyButtonWasPressed = new bool[m_NumJoyBtns];
	for(int i=0; i<m_NumJoyBtns; i++)
		m_JoyButtonWasPressed[i] = false;
}

CWinSystem::~CWinSystem()
{
	delete [] m_WasPressed;
	delete [] m_JoyButtonWasPressed;

	if(m_NumJoysticks > 0)
		SDL_JoystickClose(m_Joystick);

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
				//Resizing
				case SDL_VIDEORESIZE:
					m_Screen = SDL_SetVideoMode(event.resize.w, event.resize.h, m_BPP,
						SDL_OPENGL|SDL_RESIZABLE);
					if ( m_Screen )
					{
						m_W = m_Screen->w;
						m_H = m_Screen->h;
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

				//Quitting
				case SDL_QUIT:
					quit = true;
					break;

				//Keyboard
				case SDL_KEYDOWN:
					if(event.key.keysym.mod & KMOD_SHIFT) //shift key
						m_WasPressed[event.key.keysym.sym-32] = true;
					else
						m_WasPressed[event.key.keysym.sym] = true;
				case SDL_KEYUP:
					m_KeyState = SDL_GetKeyState(&m_NumKeys);
					break;

				//Joystick
				case SDL_JOYBUTTONDOWN:
					m_JoyButtonWasPressed[event.jbutton.button] = true;
					break;
			}
		}

		quit = quit || !loopfunc();

		if(swp)
			swapBuffers();
	}

	return 0;
}

bool CWinSystem::runLoop(CWidget *widget)
{
	int widgetmessages = widget->onResize(0, 0, m_W, m_H) | widget->onRedraw();
	swapBuffers();

	while(!(widgetmessages & WIDGET_QUIT))
	{
		widgetmessages = 0;

		SDL_Event event;

		while ( SDL_PollEvent(&event) )
		{
			switch(event.type)
			{
				//Resizing
				case SDL_VIDEORESIZE:
					m_Screen = SDL_SetVideoMode(event.resize.w, event.resize.h, m_BPP,
						SDL_OPENGL|SDL_RESIZABLE);
					if ( m_Screen )
					{
						m_W = m_Screen->w;
						m_H = m_Screen->h;
						widgetmessages |= widget->onResize(0, 0, m_W, m_H);
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

				case SDL_VIDEOEXPOSE:
					widgetmessages |= WIDGET_REDRAW;
					break;

				//Quitting
				case SDL_QUIT:
					widgetmessages |= WIDGET_QUIT | WIDGET_CANCELLED;
					break;

				//Keyboard
				case SDL_KEYDOWN:
					if(event.key.keysym.mod & KMOD_SHIFT) //shift key
						{widgetmessages |= widget->onKeyPress(event.key.keysym.sym-32);}
					else
						{widgetmessages |= widget->onKeyPress(event.key.keysym.sym);}
					break;

				//Mouse
				case SDL_MOUSEMOTION:
					widgetmessages |= widget->onMouseMove(event.motion.x, m_H - event.motion.y);
					break;

				case SDL_MOUSEBUTTONUP:
					widgetmessages |= widget->onMouseClick(event.button.x, m_H - event.button.y, event.button.button);
					break;

			}
		}

		if(widgetmessages & WIDGET_REDRAW)
		{
			widgetmessages |= widget->onRedraw();
			swapBuffers();
		}
	}

	if(widgetmessages & WIDGET_CANCELLED)
		return false;

	return true;
}

void CWinSystem::swapBuffers()
{
	SDL_GL_SwapBuffers();
}

bool CWinSystem::getKeyState(int c) const
{
	return (bool)(m_KeyState[c]);
}

bool CWinSystem::wasPressed(int c)
{
	bool ret = m_WasPressed[c];
	m_WasPressed[c] = false;
	return ret;
}

bool CWinSystem::joyBtnWasPressed(int c)
{
	bool ret = m_JoyButtonWasPressed[c];
	m_JoyButtonWasPressed[c] = false;
	return ret;
}
