/***************************************************************************
                          gamewinsystem.cpp  -  Window managing with game key settings
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

#include <cstdio>

#include "SDL.h"

#include "lconfig.h"

#include "gamewinsystem.h"

/*
Key codes used in SDL implementation:

0 .. SDLK_LAST-1:			SDL keyboard
SDLK_LAST .. SDLK_LAST+255:	First joystick
... other joysticks
(max. 8 joysticks)

So, with the defines below:
joystick x button y = JOYSTICK + JOY_NUMCODES*x + JOY_BTN + y
joystick x lefy     = JOYSTICK + JOY_NUMCODES*x + JOY_LEFT
*/

#define JOYSTICK  (SDLK_LAST)
#define JOY_LEFT  (0)
#define JOY_RIGHT (1)
#define JOY_UP    (2)
#define JOY_DOWN  (3)
#define JOY_BTN   (4)

#define JOY_NUMCODES (256)
#define MAX_NUM_JOYBUTTONS (JOY_NUMCODES - JOY_BTN)

#define MAX_NUM_JOYSTICKS (8)

vector<CGameWinSystem::SKeyName> CGameWinSystem::m_KeyNames;

CGameWinSystem::CGameWinSystem(const CString &caption) : CWinSystem(caption)
{
	//allocate the keycode arrays
	for(unsigned int p=0; p < 4; p++)
	{
		tCodeArray keys(10), controls(4);
		m_PlayerKeyCode.push_back(keys);
		m_PlayerControlCode.push_back(controls);
	}
	m_GlobalKeyCode = tCodeArray(10);

	reloadConfiguration();
}

bool CGameWinSystem::reloadConfiguration()
{
	if(!CWinSystem::reloadConfiguration()) return false;

	CLConfig &conf = *theMainConfig;

	//set default values
	m_GlobalKeyCode[eNextSong] = name2key(conf.getValue("input_global", "nextsong"));
	m_GlobalKeyCode[ePause]    = name2key(conf.getValue("input_global", "pause"));

	setupKeys("input_player0", 0);
	setupKeys("input_player1", 1);
	setupKeys("input_player2", 2);
	setupKeys("input_player3", 3);

	return true;
}

CGameWinSystem::~CGameWinSystem()
{
}

void CGameWinSystem::setupKeys(const CString &section, unsigned int player)
{
	CLConfig &conf = *theMainConfig;
	m_PlayerKeyCode[player][eShiftUp]      = name2key(conf.getValue(section, "shiftup"));
	m_PlayerKeyCode[player][eShiftDown]    = name2key(conf.getValue(section, "shiftdown"));
	m_PlayerKeyCode[player][eHorn]         = name2key(conf.getValue(section, "horn"));
	m_PlayerKeyCode[player][eCameraChange] = name2key(conf.getValue(section, "camerachange"));
	m_PlayerKeyCode[player][eCameraToggle] = name2key(conf.getValue(section, "cameratoggle"));
	m_PlayerControlCode[player][eUp]       = name2key(conf.getValue(section, "up"));
	m_PlayerControlCode[player][eDown]     = name2key(conf.getValue(section, "down"));
	m_PlayerControlCode[player][eLeft]     = name2key(conf.getValue(section, "left"));
	m_PlayerControlCode[player][eRight]    = name2key(conf.getValue(section, "right"));
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

unsigned int CGameWinSystem::getKeyFromGlobalKey(eGlobalKey key)
{
	return getGlobalKeyCode(key);
}

void CGameWinSystem::initKeyNameTranslationTable()
{
	m_KeyNames.clear();

	//single character key names:
	CString c = " ";
	for(char i = 'a'; i <= 'z'; i++)
	{
		c[0] = i;
		m_KeyNames.push_back(SKeyName(i , c));
	}
	for(char i = '0'; i <= '9'; i++)
	{
		c[0] = i;
		m_KeyNames.push_back(SKeyName(i , c));
	}
	c = "`~!@#$%^&*()-=_+[]{}\\|;\':\",./<>?";
	for(unsigned int i=0; i < c.length(); i++)
	{
		m_KeyNames.push_back(SKeyName(c[i] , c.mid(i, 1)));
	}

	//misc keys
	m_KeyNames.push_back(SKeyName(SDLK_RETURN   , "return"));
	m_KeyNames.push_back(SKeyName(' '           , "space"));
	m_KeyNames.push_back(SKeyName(SDLK_TAB      , "tab"));
	m_KeyNames.push_back(SKeyName(SDLK_ESCAPE   , "escape"));
	m_KeyNames.push_back(SKeyName(SDLK_BACKSPACE, "backspace"));

	//modifier keys
	m_KeyNames.push_back(SKeyName(SDLK_LCTRL , "lctrl"));
	m_KeyNames.push_back(SKeyName(SDLK_RCTRL , "rctrl"));
	m_KeyNames.push_back(SKeyName(SDLK_LSHIFT, "lshift"));
	m_KeyNames.push_back(SKeyName(SDLK_RSHIFT, "rshift"));
	m_KeyNames.push_back(SKeyName(SDLK_LALT  , "lalt"));
	m_KeyNames.push_back(SKeyName(SDLK_RALT  , "ralt"));
	m_KeyNames.push_back(SKeyName(SDLK_LMETA , "lmeta"));
	m_KeyNames.push_back(SKeyName(SDLK_RMETA , "rmeta"));

	//keypad
	m_KeyNames.push_back(SKeyName(SDLK_KP0, "kp0"));
	m_KeyNames.push_back(SKeyName(SDLK_KP1, "kp1"));
	m_KeyNames.push_back(SKeyName(SDLK_KP2, "kp2"));
	m_KeyNames.push_back(SKeyName(SDLK_KP3, "kp3"));
	m_KeyNames.push_back(SKeyName(SDLK_KP4, "kp4"));
	m_KeyNames.push_back(SKeyName(SDLK_KP5, "kp5"));
	m_KeyNames.push_back(SKeyName(SDLK_KP6, "kp6"));
	m_KeyNames.push_back(SKeyName(SDLK_KP7, "kp7"));
	m_KeyNames.push_back(SKeyName(SDLK_KP8, "kp8"));
	m_KeyNames.push_back(SKeyName(SDLK_KP9, "kp9"));
	m_KeyNames.push_back(SKeyName(SDLK_KP_ENTER   , "kpenter"));
	m_KeyNames.push_back(SKeyName(SDLK_KP_DIVIDE  , "kp/"));
	m_KeyNames.push_back(SKeyName(SDLK_KP_MULTIPLY, "kp*"));
	m_KeyNames.push_back(SKeyName(SDLK_KP_PLUS    , "kp+"));
	m_KeyNames.push_back(SKeyName(SDLK_KP_MINUS   , "kp-"));
	m_KeyNames.push_back(SKeyName(SDLK_KP_PERIOD  , "kp."));
	m_KeyNames.push_back(SKeyName(SDLK_KP_EQUALS  , "kp="));

	//navigation island
	m_KeyNames.push_back(SKeyName(SDLK_INSERT  , "insert"));
	m_KeyNames.push_back(SKeyName(SDLK_DELETE  , "delete"));
	m_KeyNames.push_back(SKeyName(SDLK_HOME    , "home"));
	m_KeyNames.push_back(SKeyName(SDLK_END     , "end"));
	m_KeyNames.push_back(SKeyName(SDLK_PAGEUP  , "pageup"));
	m_KeyNames.push_back(SKeyName(SDLK_PAGEDOWN, "pagedown"));

	m_KeyNames.push_back(SKeyName(SDLK_PAUSE  , "pause"));

	//function keys
	m_KeyNames.push_back(SKeyName(SDLK_F1 , "f1"));
	m_KeyNames.push_back(SKeyName(SDLK_F2 , "f2"));
	m_KeyNames.push_back(SKeyName(SDLK_F3 , "f3"));
	m_KeyNames.push_back(SKeyName(SDLK_F4 , "f4"));
	m_KeyNames.push_back(SKeyName(SDLK_F5 , "f5"));
	m_KeyNames.push_back(SKeyName(SDLK_F6 , "f6"));
	m_KeyNames.push_back(SKeyName(SDLK_F7 , "f7"));
	m_KeyNames.push_back(SKeyName(SDLK_F8 , "f8"));
	m_KeyNames.push_back(SKeyName(SDLK_F9 , "f9"));
	m_KeyNames.push_back(SKeyName(SDLK_F10, "f10"));
	m_KeyNames.push_back(SKeyName(SDLK_F11, "f11"));
	m_KeyNames.push_back(SKeyName(SDLK_F12, "f12"));

	//cursor keys
	m_KeyNames.push_back(SKeyName(SDLK_UP   , "up"));
	m_KeyNames.push_back(SKeyName(SDLK_DOWN , "down"));
	m_KeyNames.push_back(SKeyName(SDLK_LEFT , "left"));
	m_KeyNames.push_back(SKeyName(SDLK_RIGHT, "right"));

	//joysticks
	for(unsigned int i=0; i < MAX_NUM_JOYSTICKS; i++)
	{
		m_KeyNames.push_back(SKeyName(
			JOYSTICK + JOY_NUMCODES*i + JOY_UP,
			CString().format("joy%dup", 16, i)));
		m_KeyNames.push_back(SKeyName(
			JOYSTICK + JOY_NUMCODES*i + JOY_DOWN,
			CString().format("joy%ddown", 16, i)));
		m_KeyNames.push_back(SKeyName(
			JOYSTICK + JOY_NUMCODES*i + JOY_LEFT,
			CString().format("joy%dleft", 16, i)));
		m_KeyNames.push_back(SKeyName(
			JOYSTICK + JOY_NUMCODES*i + JOY_RIGHT,
			CString().format("joy%dright", 16, i)));

		for(unsigned int j=0; j < MAX_NUM_JOYBUTTONS; j++)
			m_KeyNames.push_back(SKeyName(
				JOYSTICK + JOY_NUMCODES*i + JOY_BTN+j ,
				CString().format("joy%dbutton%d", 32, i, j)
				));
	}
}

CString CGameWinSystem::key2name(unsigned int key) const
{
	if(m_KeyNames.size() == 0) initKeyNameTranslationTable();

	for(unsigned int i=0; i < m_KeyNames.size(); i++)
		if(m_KeyNames[i].key == (int)key)
			return m_KeyNames[i].name;

	printf("Key %d is unknown\n", key);
	return ""; //not found
}

unsigned int CGameWinSystem::name2key(const CString &name) const
{
	if(m_KeyNames.size() == 0) initKeyNameTranslationTable();

	for(unsigned int i=0; i < m_KeyNames.size(); i++)
		if(m_KeyNames[i].name == name)
			return m_KeyNames[i].key;

	return 0; //not found
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
	//SDL keys
	if(code < SDLK_LAST)
	{
		return getKeyState(code);
	}

	//Joysticks
	int joyID = (code - JOYSTICK) / JOY_NUMCODES;
	if(
		joyID >= 0 &&
		joyID < MAX_NUM_JOYSTICKS &&
		(unsigned int)joyID < m_Joysticks.size() &&
		m_Joysticks[joyID].joystick != NULL
		)
	{
		int button = code - JOYSTICK - JOY_NUMCODES*joyID - JOY_BTN;
		if(button < 0)
			return false;

		return SDL_JoystickGetButton(m_Joysticks[joyID].joystick, button);
	}

	return false;
}

bool CGameWinSystem::keyCodeWasPressed(unsigned int code)
{
	//SDL keys
	if(code < SDLK_LAST)
		return wasPressed(code);

	//Joysticks
	int joyID = (code - JOYSTICK) / JOY_NUMCODES;
	if(joyID >= 0 && joyID < MAX_NUM_JOYSTICKS)
	{
		int button = code - JOYSTICK - JOY_NUMCODES*joyID - JOY_BTN;
		if(button < 0)
			return false;

		return joyBtnWasPressed(joyID, button);
	}

	return false;
}

float CGameWinSystem::getControlCodeState(unsigned int code)
{
	//SDL keys
	if(code < SDLK_LAST)
	{
		return (float)(getKeyState(code));
	}

	//Joysticks
	int joyID = (code - JOYSTICK) / JOY_NUMCODES;
	if(
		joyID >= 0 &&
		joyID < MAX_NUM_JOYSTICKS &&
		(unsigned int)joyID < m_Joysticks.size() &&
		m_Joysticks[joyID].joystick != NULL
		)
	{
		SDL_Joystick *js = m_Joysticks[joyID].joystick;
		int direction = code - JOYSTICK - JOY_NUMCODES*joyID;
		if(direction == JOY_LEFT)
		{
			float val = (float)-SDL_JoystickGetAxis(js, 0) / 32767;
			if(val < 0.0) val = 0.0;
			return val;
		}
		else if(direction == JOY_RIGHT)
		{
			float val = (float)SDL_JoystickGetAxis(js, 0) / 32767;
			if(val < 0.0) val = 0.0;
			return val;
		}
		else if(direction == JOY_UP)
		{
			float val = (float)-SDL_JoystickGetAxis(js, 1) / 32767;
			if(val < 0.0) val = 0.0;
			return val;
		}
		else if(direction == JOY_DOWN)
		{
			float val = (float)SDL_JoystickGetAxis(js, 1) / 32767;
			if(val < 0.0) val = 0.0;
			return val;
		}
		//TODO: support joystick buttons for controls
	}

	return 0.0;
}

unsigned int CGameWinSystem::getJoystickKeyCode(unsigned int joystick, unsigned int button)
{
	return JOYSTICK + JOY_NUMCODES*joystick + JOY_BTN + button;
}

unsigned int CGameWinSystem::getJoystickAxisCode(unsigned int joystick, unsigned int axis, bool positive)
{
	unsigned int offset = JOYSTICK + JOY_NUMCODES*joystick;

	if(axis == 0 &&  positive) return offset + JOY_RIGHT;
	if(axis == 0 && !positive) return offset + JOY_LEFT;
	if(axis == 1 && !positive) return offset + JOY_UP;
	if(axis == 1 &&  positive) return offset + JOY_DOWN;

	return 0;
}

