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

#include <vector> //STL vector template
namespace std {}
using namespace std;

#include "SDL.h"

#include "cstring.h"
#include "usmacros.h"
#include "widget.h"

/**
  *@author CJP
  */

class CWinSystem {
public: 
	CWinSystem(const CString &caption);
	virtual ~CWinSystem();

	int runLoop( bool (CALLBACKFUN *loopfunc)(), bool swp = false);
	bool runLoop(CWidget *widget);

	virtual bool reloadConfiguration();

	void swapBuffers();

	bool getKeyState(int c) const;
	bool wasPressed(int c);
	bool joyBtnWasPressed(int js, int c);

	void showMouseCursor(bool show=true);

	SDL_Surface *getScreen(){return m_Screen;}

	unsigned int getWidth() const
		{return m_W;}
	unsigned int getHeight() const
		{return m_H;}
	unsigned int getBPP() const
		{return m_BPP;}

protected:
	void drawCursor();

	bool m_OpenGLCursor;

	unsigned int m_W, m_H, m_BPP;
	bool m_Fullscreen;
	Uint32 m_Flags;

	unsigned int m_DefaultW, m_DefaultH;

	int m_VisibleTiles;

	SDL_Surface *m_Screen;

	//keyboard
	Uint8 *m_KeyState;
	bool *m_WasPressed;
	int m_NumKeys;

	//joystick
	struct SJoystick
	{
		SDL_Joystick *joystick;
		unsigned int numButtons;
		bool *buttonWasPressed;
	};
	vector<SJoystick> m_Joysticks;

	/*
	Emulation of normal keys for joystick buttons.
	Actual implementation is only in CGameWinsystem.
	*/
	virtual unsigned int getJoystickKeyCode(unsigned int joystick, unsigned int button);
	virtual unsigned int getJoystickAxisCode(unsigned int joystick, unsigned int axis, bool positive);
};

extern CWinSystem *theWinSystem;

#endif
