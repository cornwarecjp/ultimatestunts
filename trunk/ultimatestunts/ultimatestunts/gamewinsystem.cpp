/***************************************************************************
                          gamewinsystem.cpp  -  description
                             -------------------
    begin                : do sep 16 2004
    copyright            : (C) 2004 by CJP
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

#include "SDL.h"
#include "gamewinsystem.h"

/*
Key codes used in SDL implementation:

0 .. SDLK_LAST-1:			SDL keyboard
SDLK_LAST .. SDLK_LAST+256:	First joystick
... other joysticks

*/

#define JOY0_LEFT (SDLK_LAST)
#define JOY0_RIGHT (SDLK_LAST+1)
#define JOY0_UP (SDLK_LAST+2)
#define JOY0_DOWN (SDLK_LAST+3)

#define JOY0_BTN0 (SDLK_LAST+4)
#define JOY0_BTN1 (SDLK_LAST+5)
#define JOY0_BTN2 (SDLK_LAST+6)
#define JOY0_BTN3 (SDLK_LAST+7)


CGameWinSystem::CGameWinSystem(const CString &caption, const CLConfig &conf) : CWinSystem(caption, conf)
{
	//allocate the keycode arrays
	for(unsigned int p=0; p < 4; p++)
	{
		tCodeArray keys(10), controls(4);
		m_PlayerKeyCode.push_back(keys);
		m_PlayerControlCode.push_back(controls);
	}
	m_GlobalKeyCode = tCodeArray(10);

	//set default values
	m_GlobalKeyCode[eExit] = getKeyCodeFromString(conf.getValue("input_global", "exit"));
	m_GlobalKeyCode[eNextSong] = getKeyCodeFromString(conf.getValue("input_global", "nextsong"));

	setupKeys(conf, "input_player0", 0);
	setupKeys(conf, "input_player1", 1);
	setupKeys(conf, "input_player2", 2);
	setupKeys(conf, "input_player3", 3);
}

CGameWinSystem::~CGameWinSystem()
{
}

void CGameWinSystem::setupKeys(const CLConfig &conf, const CString &section, unsigned int player)
{
	m_PlayerKeyCode[player][eShiftUp] = getKeyCodeFromString(conf.getValue(section, "shiftup"));
	m_PlayerKeyCode[player][eShiftDown] = getKeyCodeFromString(conf.getValue(section, "shiftdown"));
	m_PlayerKeyCode[player][eHorn] = getKeyCodeFromString(conf.getValue(section, "horn"));
	m_PlayerKeyCode[player][eCameraChange] = getKeyCodeFromString(conf.getValue(section, "camerachange"));
	m_PlayerKeyCode[player][eCameraToggle] = getKeyCodeFromString(conf.getValue(section, "cameratoggle"));
	m_PlayerControlCode[player][eUp] = getKeyCodeFromString(conf.getValue(section, "up"));
	m_PlayerControlCode[player][eDown] = getKeyCodeFromString(conf.getValue(section, "down"));
	m_PlayerControlCode[player][eLeft] = getKeyCodeFromString(conf.getValue(section, "left"));
	m_PlayerControlCode[player][eRight] = getKeyCodeFromString(conf.getValue(section, "right"));
}


// Public interface:

float CGameWinSystem::getPlayerControlState(ePlayerControl ctl, unsigned int player)
{
	return getControlCodeState(getPlayerControlCode(ctl, player));
}

bool CGameWinSystem::getPlayerKeyState(ePlayerKey key, unsigned int player)
{
	return getKeyCodeState(getPlayerKeyCode(key, player));
}

bool CGameWinSystem::playerKeyWasPressed(ePlayerKey key, unsigned int player)
{
	return keyCodeWasPressed(getPlayerKeyCode(key, player));
}

bool CGameWinSystem::getGlobalKeyState(eGlobalKey key)
{
	return getKeyCodeState(getGlobalKeyCode(key));
}

bool CGameWinSystem::globalKeyWasPressed(eGlobalKey key)
{
	return keyCodeWasPressed(getGlobalKeyCode(key));
}


//String to keycode conversion
unsigned int CGameWinSystem::getKeyCodeFromString(const CString &s)
{
	if(s.length() == 0) return 0;

	//single character
	if(s.length() == 1) return s[0];

	//misc keys
	if(s == "return") return SDLK_RETURN;
	if(s == "space") return ' ';
	if(s == "tab") return SDLK_TAB;
	if(s == "escape") return SDLK_ESCAPE;
	if(s == "backspace") return SDLK_BACKSPACE;

	//modifier keys
	if(s == "lctrl") return SDLK_LCTRL;
	if(s == "rctrl") return SDLK_RCTRL;
	if(s == "lshift") return SDLK_LSHIFT;
	if(s == "rshift") return SDLK_RSHIFT;
	if(s == "lalt") return SDLK_LALT;
	if(s == "ralt") return SDLK_RALT;
	if(s == "lmeta") return SDLK_LMETA;
	if(s == "rmeta") return SDLK_RMETA;

	//keypad
	if(s == "kp0") return SDLK_KP0;
	if(s == "kp1") return SDLK_KP1;
	if(s == "kp2") return SDLK_KP2;
	if(s == "kp3") return SDLK_KP3;
	if(s == "kp4") return SDLK_KP4;
	if(s == "kp5") return SDLK_KP5;
	if(s == "kp6") return SDLK_KP6;
	if(s == "kp7") return SDLK_KP7;
	if(s == "kp8") return SDLK_KP8;
	if(s == "kp9") return SDLK_KP9;
	if(s == "kpenter") return SDLK_KP_ENTER;
	if(s == "kp/") return SDLK_KP_DIVIDE;
	if(s == "kp*") return SDLK_KP_MULTIPLY;
	if(s == "kp+") return SDLK_KP_PLUS;
	if(s == "kp-") return SDLK_KP_MINUS;
	if(s == "kp.") return SDLK_KP_PERIOD;
	if(s == "kp=") return SDLK_KP_EQUALS;

	//navigation island
	if(s == "insert") return SDLK_INSERT;
	if(s == "delete") return SDLK_DELETE;
	if(s == "home") return SDLK_HOME;
	if(s == "end") return SDLK_END;
	if(s == "pageup") return SDLK_PAGEUP;
	if(s == "pagedown") return SDLK_PAGEDOWN;

	//function keys
	if(s == "f1") return SDLK_F1;
	if(s == "f2") return SDLK_F2;
	if(s == "f3") return SDLK_F3;
	if(s == "f4") return SDLK_F4;
	if(s == "f5") return SDLK_F5;
	if(s == "f6") return SDLK_F6;
	if(s == "f7") return SDLK_F7;
	if(s == "f8") return SDLK_F8;
	if(s == "f9") return SDLK_F9;
	if(s == "f10") return SDLK_F10;
	if(s == "f11") return SDLK_F11;
	if(s == "f12") return SDLK_F12;

	//cursor keys
	if(s == "up") return SDLK_UP;
	if(s == "down") return SDLK_DOWN;
	if(s == "left") return SDLK_LEFT;
	if(s == "right") return SDLK_RIGHT;

	//first joystick
	if(s == "joy0up") return JOY0_UP;
	if(s == "joy0down") return JOY0_DOWN;
	if(s == "joy0left") return JOY0_LEFT;
	if(s == "joy0right") return JOY0_RIGHT;
	if(s == "joy0button0") return JOY0_BTN0;
	if(s == "joy0button1") return JOY0_BTN1;
	if(s == "joy0button2") return JOY0_BTN2;
	if(s == "joy0button3") return JOY0_BTN3;

	//unknown string
	return 0;
}

//Enum to keycode conversion

unsigned int CGameWinSystem::getGlobalKeyCode(eGlobalKey key)
{
	return m_GlobalKeyCode[key];
}

unsigned int CGameWinSystem::getPlayerKeyCode(ePlayerKey key, unsigned int player)
{
	return m_PlayerKeyCode[player][key];
}

unsigned int CGameWinSystem::getPlayerControlCode(ePlayerControl ctl, unsigned int player)
{
	return m_PlayerControlCode[player][ctl];
}



//Get state from keycode

bool CGameWinSystem::getKeyCodeState(unsigned int code)
{
	if(code < SDLK_LAST)
	{
		return getKeyState(code);
	}

	if(m_NumJoysticks > 0)
	{
		if(code == JOY0_BTN0)
			return SDL_JoystickGetButton(m_Joystick, 0);
		if(code == JOY0_BTN1)
			return SDL_JoystickGetButton(m_Joystick, 1);
		if(code == JOY0_BTN2)
			return SDL_JoystickGetButton(m_Joystick, 2);
		if(code == JOY0_BTN3)
			return SDL_JoystickGetButton(m_Joystick, 3);
	}

	return false;
}

bool CGameWinSystem::keyCodeWasPressed(unsigned int code)
{
	if(code < SDLK_LAST)
		return wasPressed(code);
	if(code == JOY0_BTN0)
		return joyBtnWasPressed(0);
	if(code == JOY0_BTN1)
		return joyBtnWasPressed(1);
	if(code == JOY0_BTN2)
		return joyBtnWasPressed(2);
	if(code == JOY0_BTN3)
		return joyBtnWasPressed(3);

	return false;
}

float CGameWinSystem::getControlCodeState(unsigned int code)
{
	if(code < SDLK_LAST)
	{
		return (float)(getKeyState(code));
	}
	else if(m_NumJoysticks > 0)
	{
		if(code == JOY0_LEFT)
		{
			float val = (float)-SDL_JoystickGetAxis(m_Joystick, 0) / 32767;
			if(val < 0.0) val = 0.0;
			return val;
		}
		else if(code == JOY0_RIGHT)
		{
			float val = (float)SDL_JoystickGetAxis(m_Joystick, 0) / 32767;
			if(val < 0.0) val = 0.0;
			return val;
		}
		else if(code == JOY0_UP)
		{
			float val = (float)-SDL_JoystickGetAxis(m_Joystick, 1) / 32767;
			if(val < 0.0) val = 0.0;
			return val;
		}
		else if(code == JOY0_DOWN)
		{
			float val = (float)SDL_JoystickGetAxis(m_Joystick, 1) / 32767;
			if(val < 0.0) val = 0.0;
			return val;
		}
	}

	return 0.0;
}
