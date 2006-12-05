/***************************************************************************
                          winsystem.h  -  A window managing class
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

#ifndef WINSYSTEM_H
#define WINSYSTEM_H

#include "SDL.h"

#include "cstring.h"
#include "usmacros.h"
#include "lconfig.h"
#include "widget.h"

/**
  *@author CJP
  */

class CWinSystem {
public: 
	CWinSystem(const CString &caption, const CLConfig &conf);
	virtual ~CWinSystem();

	int runLoop( bool (CALLBACKFUN *loopfunc)(), bool swp = false);
	bool runLoop(CWidget *widget);

	virtual bool reloadConfiguration();

	void swapBuffers();

	bool getKeyState(int c) const;
	bool wasPressed(int c);
	bool joyBtnWasPressed(int c);

	SDL_Surface *getScreen(){return m_Screen;}

	unsigned int getWidth() const
		{return m_W;}
	unsigned int getHeight() const
		{return m_H;}
	unsigned int getBPP() const
		{return m_BPP;}
protected:
	unsigned int m_W, m_H, m_BPP;
	bool m_Fullscreen;
	Uint32 m_Flags;

	int m_VisibleTiles;

	SDL_Surface *m_Screen;

	//keyboard
	Uint8 *m_KeyState;
	bool *m_WasPressed;
	int m_NumKeys;

	//joystick
	bool *m_JoyButtonWasPressed;
	int m_NumJoyBtns;

	int m_NumJoysticks;
	SDL_Joystick *m_Joystick;
};

extern CWinSystem *theWinSystem;

#endif
