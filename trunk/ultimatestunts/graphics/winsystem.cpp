/***************************************************************************
                          winsystem.cpp  -  description
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

SDL_Surface *screen;

#include "winsystem.h"

CWinSystem::CWinSystem()
{
	//Code coming from SDL gears

  //Uint8 *keys;

  SDL_Init(SDL_INIT_VIDEO);

  screen = SDL_SetVideoMode(300, 300, 16, SDL_OPENGL|SDL_RESIZABLE);
  if ( ! screen ) {
    fprintf(stderr, "Couldn't set 300x300 GL video mode: %s\n", SDL_GetError());
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
