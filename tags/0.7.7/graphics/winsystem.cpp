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
#include <cmath>

#include "lconfig.h"
#include "usmacros.h"
#include "timer.h"

#include "mousecursor.h"

#include "winsystem.h"

CWinSystem *theWinSystem = NULL;

bool dummy_loopfunc()
{return false;} //exit immediately

CWinSystem::CWinSystem(const CString &caption)
{
	theWinSystem = this;

	//Default values:
	m_Flags = SDL_OPENGL |SDL_RESIZABLE|SDL_ANYFORMAT;
	m_W = 0; m_H = 0; //to make sure that the format is changed the first time
	m_BPP = 24;
	m_VisibleTiles = 20;
	m_OpenGLCursor = true;

	//Some code coming from SDL gears
	//This has to be called before video mode initialization
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER);

	//Minimum values:
	//This has to be called before video mode initialization
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

	//Determine original screen size:
	{
		SDL_Rect **modes = SDL_ListModes(NULL, SDL_OPENGL | SDL_FULLSCREEN | SDL_ANYFORMAT);

		/* Check is there are any modes available */
		if(modes == NULL)
		{
			printf("    No modes available!\n");
			exit(-1);
		}
		
		/* Check if or resolution is restricted */
		if(modes == (SDL_Rect **)-1)
		{
			printf("    All resolutions available\n");

			m_DefaultW = 1024;
			m_DefaultH = 768;
		}
		else
		{
			/* Print valid modes */
			printf("    Available Modes:\n");
			for(unsigned int i=0; modes[i] != NULL; i++)
				printf("      %d x %d\n", modes[i]->w, modes[i]->h);

			m_DefaultW = modes[0]->w;
			m_DefaultH = modes[0]->h;
		}
	}

	reloadConfiguration(); //sets video mode and misc. settings

	SDL_WM_SetCaption(caption.c_str(), "ultimatestunts");

	unsigned int numJoysticks = SDL_NumJoysticks();
	printf("   Found %d joysticks\n", numJoysticks);

	if(numJoysticks > 0)
	{
		SDL_JoystickEventState(SDL_ENABLE);

		m_Joysticks.resize(numJoysticks);

		//Open joysticks:
		for(unsigned int i=0; i < numJoysticks; i++)
		{
			SDL_Joystick *js = SDL_JoystickOpen(i);
			m_Joysticks[i].joystick = js;
			if(m_Joysticks[i].joystick)
			{
				printf("   Opened Joystick %d\n", i);
				printf("   Name: %s\n", SDL_JoystickName(i));
				printf("   Number of Axes: %d\n", SDL_JoystickNumAxes(js));
				printf("   Number of Buttons: %d\n", SDL_JoystickNumButtons(js));
				printf("   Number of Balls: %d\n", SDL_JoystickNumBalls(js));

				m_Joysticks[i].numButtons = SDL_JoystickNumButtons(js);
				m_Joysticks[i].buttonWasPressed = new bool[m_Joysticks[i].numButtons];
			}
			else
			{
				printf("   Couldn't open Joystick %d\n", i);

				m_Joysticks[i].numButtons = 0;
				m_Joysticks[i].buttonWasPressed = NULL;
			}
		}
	}

	runLoop(dummy_loopfunc, true); //catch startup-events

	m_KeyState = SDL_GetKeyState(&m_NumKeys);
	m_WasPressed = new bool[m_NumKeys];
	for(int i=0; i<m_NumKeys; i++)
		m_WasPressed[i] = false;

	for(unsigned int i = 0; i < m_Joysticks.size(); i++)
		for(unsigned int j = 0; j < m_Joysticks[i].numButtons; j++)
			m_Joysticks[i].buttonWasPressed[j] = false;
}

bool CWinSystem::reloadConfiguration()
{
	CLConfig &conf = *theMainConfig;

	//Display variable:
	unsigned int newW = m_W;
	unsigned int newH = m_H;
	unsigned int newFlags = m_Flags;

	CString cnf = conf.getValue("graphics","display");
	printf("   Display variable: \"%s\"\n", cnf.c_str());

	//Resolution
	newW = m_DefaultW;
	newH = m_DefaultH;
	int pos = cnf.inStr(':');
	if(pos > 0) //There is a ':' in cnf_display
	{
		CString s = cnf.mid(pos+1, cnf.length()-pos);
		cnf = cnf.mid(0, pos);
		pos = s.inStr('x');
		if(pos > 0) //There is a 'x' in the rest
		{
			newW = s.mid(0, pos).toInt();
			newH = s.mid(pos+1, s.length()-pos).toInt();

			//TODO: support bpp setting
		}
	}

	//Flags
#ifdef WINDOWRESIZE
	newFlags = SDL_OPENGL | SDL_RESIZABLE | SDL_ANYFORMAT;
#else
	newFlags = SDL_OPENGL | SDL_ANYFORMAT;
#endif
	if(cnf == "fullscreen")
		newFlags = SDL_OPENGL | SDL_FULLSCREEN | SDL_ANYFORMAT;

	//Set video mode (if changed)
	if( (newW != m_W) || (newH != m_H) || (newFlags != m_Flags) )
	{
		printf("   Setting resolution to %dx%d:%d...\n", newW, newH, m_BPP);
		m_Screen = SDL_SetVideoMode(newW, newH, m_BPP, newFlags);
		if ( ! m_Screen )
		{
			printf("   Couldn't set new %dx%d GL video mode: %s\n",
				newW, newH, SDL_GetError());

			//Try to restore old mode
			m_Screen = SDL_SetVideoMode(m_W, m_H, m_BPP, m_Flags);
			if ( ! m_Screen )
			{
				printf("   Couldn't set back old %dx%d GL video mode: %s\n",
					m_W, m_H, SDL_GetError());
				SDL_Quit();
				exit(2);
			}
			m_W = m_Screen->w;
			m_H = m_Screen->h;
			m_BPP = m_Screen->format->BitsPerPixel;
			return false;

		}
		m_W = m_Screen->w;
		m_H = m_Screen->h;
		m_Flags = newFlags;
		m_BPP = m_Screen->format->BitsPerPixel;
	}
	printf("   ...Now working at %dx%d:%d\n", m_W, m_H, m_BPP);


	//visible_tiles variable
	cnf = conf.getValue("graphics", "visible_tiles");
	if(cnf != "")
		m_VisibleTiles = cnf.toInt();

	//OpenGL cursor
	cnf = conf.getValue("user_interface", "opengl_mousecursor");
	if(cnf != "")
		m_OpenGLCursor = (cnf== "true");

	if(m_OpenGLCursor)
		{SDL_ShowCursor(SDL_DISABLE);}
	else
		{SDL_ShowCursor(SDL_ENABLE);}

	return true;
}

CWinSystem::~CWinSystem()
{
	delete [] m_WasPressed;

	for(unsigned int i=0; i < m_Joysticks.size(); i++)
		if(m_Joysticks[i].joystick != NULL)
		{
			delete [] m_Joysticks[i].buttonWasPressed;
			SDL_JoystickClose(m_Joysticks[i].joystick);
		}

	SDL_Quit();
}

int CWinSystem::runLoop( bool (CALLBACKFUN *loopfunc)(), bool swp)
{
	bool quit = false;

	SDL_EnableKeyRepeat(0, 0);

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
				{
					SJoystick &js = m_Joysticks[event.jbutton.which];
					if(js.buttonWasPressed != NULL)
						js.buttonWasPressed[event.jbutton.button] = true;
					break;
				}
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
	drawCursor();
	swapBuffers();

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

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
					{
						if(event.key.keysym.sym >= 'a' && event.key.keysym.sym <= 'z') //make uppercase
						{
							widgetmessages |= widget->onKeyPress(event.key.keysym.sym-32);
							break;
						}

						//The following assumes US-style keyboard layout
						//TODO: find the correct way to do this
						switch(event.key.keysym.sym)
						{
						case ';': widgetmessages |= widget->onKeyPress(':'); break;
						case '-': widgetmessages |= widget->onKeyPress('_'); break;
						case '=': widgetmessages |= widget->onKeyPress('+'); break;
						case ',': widgetmessages |= widget->onKeyPress('<'); break;
						case '.': widgetmessages |= widget->onKeyPress('>'); break;
						case '/': widgetmessages |= widget->onKeyPress('?'); break;
						case '9': widgetmessages |= widget->onKeyPress('('); break;
						case '0': widgetmessages |= widget->onKeyPress(')'); break;
						default:
							widgetmessages |= widget->onKeyPress(event.key.keysym.sym);
							break;
						}
					}
					else
						{widgetmessages |= widget->onKeyPress(event.key.keysym.sym);}
					break;

				//Mouse
				case SDL_MOUSEMOTION:
					widgetmessages |= widget->onMouseMove(
						event.motion.x, m_H - event.motion.y, event.motion.state);
					if(m_OpenGLCursor) widgetmessages |= WIDGET_REDRAW;
					break;

				case SDL_MOUSEBUTTONUP:
					widgetmessages |= widget->onMouseClick(event.button.x, m_H - event.button.y, event.button.button);
					break;

				//Joystick events, emulating key events:
				case SDL_JOYBUTTONDOWN:
				{
					unsigned int keycode = getJoystickKeyCode(
						event.jbutton.which, event.jbutton.button);
					if(keycode != 0)
						widgetmessages |= widget->onKeyPress(keycode);
					break;
				}
				case SDL_JOYAXISMOTION:
				{
					float x = 0.0, y = 0.0;
					if(event.jaxis.axis == 0) x = float(event.jaxis.value) / 32768.0;
					if(event.jaxis.axis == 1) y = float(event.jaxis.value) / 32768.0;

					static bool wasDownX = false, wasDownY = false;

					if(fabsf(x) > 0.75)
					{
						if(!wasDownX)
						{
							wasDownX = true;
							unsigned int keycode = getJoystickAxisCode(
								event.jaxis.which, 0, x > 0.0);
							widgetmessages |= widget->onKeyPress(keycode);
						}
					}
					else
						{wasDownX = false;}

					if(fabsf(y) > 0.75)
					{
						if(!wasDownY)
						{
							wasDownY = true;
							unsigned int keycode = getJoystickAxisCode(
								event.jaxis.which, 1, y > 0.0);
							widgetmessages |= widget->onKeyPress(keycode);
						}
					}
					else
						{wasDownY = false;}

					break;
				}
			}
		}

		widgetmessages |= widget->onIdle();

		if(widgetmessages & WIDGET_REDRAW)
		{
			widgetmessages |= widget->onRedraw();
			drawCursor();
			swapBuffers();
		}
	}

	SDL_EnableKeyRepeat(0, 0);

	if(widgetmessages & WIDGET_CANCELLED)
		return false;

	return true;
}

void CWinSystem::swapBuffers()
{
	SDL_GL_SwapBuffers();
}

void CWinSystem::showMouseCursor(bool show)
{
	if(show && !m_OpenGLCursor)
		{SDL_ShowCursor(SDL_ENABLE);}
	else
		{SDL_ShowCursor(SDL_DISABLE);}
}

void CWinSystem::drawCursor()
{
	if(!m_OpenGLCursor) return;

	static CMouseCursor cursor;

	int x, y;
	SDL_GetMouseState(&x, &y);

	glLoadIdentity();
	glTranslatef(x, m_H-y, 0);

	glScissor(0, 0, m_W, m_H);

	//the rectangle
	glColor3f(1,1,1);
	cursor.draw();
	glBegin(GL_QUADS);
	glTexCoord2f(0,1); glVertex2f( 0,-31);
	glTexCoord2f(1,1); glVertex2f(31,-31);
	glTexCoord2f(1,0); glVertex2f(31, 0 );
	glTexCoord2f(0,0); glVertex2f( 0, 0 );
	glEnd();
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

bool CWinSystem::joyBtnWasPressed(int js, int c)
{
	if((unsigned int)js >= m_Joysticks.size() ||
		m_Joysticks[js].buttonWasPressed == NULL ||
		(unsigned int)c >= m_Joysticks[js].numButtons)
			return false;

	bool ret = m_Joysticks[js].buttonWasPressed[c];
	m_Joysticks[js].buttonWasPressed[c] = false;
	return ret;
}

unsigned int CWinSystem::getJoystickKeyCode(unsigned int joystick, unsigned int button)
{
	return 0; //to be overloaded in a derived class
}

unsigned int CWinSystem::getJoystickAxisCode(unsigned int joystick, unsigned int axis, bool positive)
{
	return 0; //to be overloaded in a derived class
}
