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
#include "SDL/SDL.h"

#include <stdio.h>

SDL_Surface *screen;

#include "winsystem.h"

CWinSystem::CWinSystem(const CLConfig &conf)
{
	//Default values:
	Uint32 flags = SDL_OPENGL|SDL_RESIZABLE;
	int w = 640, h = 480, bpp = 24;

	CString cnf_display = conf.getValue("graphics","display");
	printf("Display variable: \"%s\"\n", cnf_display.c_str());
	unsigned int pos = cnf_display.inStr(':');
	if(pos > 0 && pos < cnf_display.length()-1) //There is a ':' in cnf_display
	{
		CString s = cnf_display.mid(pos+1, cnf_display.length()-pos);
		cnf_display = cnf_display.mid(0, pos);


		pos = s.inStr('x');
		if(pos > 0 && pos < s.length()-1)
		{
			w = s.mid(0, pos).toInt();
			h = s.mid(pos+1, s.length()-pos).toInt();
			//TODO: support bpp setting
		}
	}

	if(cnf_display == "fullscreen")
		flags = SDL_OPENGL|SDL_FULLSCREEN;

	//Some code coming from SDL gears

  SDL_Init(SDL_INIT_VIDEO);

  screen = SDL_SetVideoMode(w, h, bpp, flags);
  if ( ! screen ) {
    fprintf(stderr, "Couldn't set %dx%d GL video mode: %s\n", w, h, SDL_GetError());
    SDL_Quit();
    exit(2);
  }
  SDL_WM_SetCaption("UltimateStunts", "ultimatestunts");

}
CWinSystem::~CWinSystem()
{
	SDL_Quit();
}

int CWinSystem::runLoop( bool (CALLBACKFUN *loopfunc)() )
{
	bool quit = false;

	//if loopfunc want to quit for some reason, then is returns false
	while(loopfunc() && !quit)
	{
		SDL_Event event;

		while ( SDL_PollEvent(&event) )
		{
			switch(event.type)
			{
				case SDL_VIDEORESIZE:
					screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 16,
						SDL_OPENGL|SDL_RESIZABLE);
					if ( screen )
					{
						reshape(screen->w, screen->h);
					} else {
						/* Uh oh, we couldn't set the new video mode?? */;
					}
					break;

				case SDL_QUIT:
					quit = true;
					break;
			}
		}

	}

	return 0;
}

void CWinSystem::reshape(int w, int h)
{
	; //Do some openGL stuff to adapt rendering to new window size
}
