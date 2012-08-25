/***************************************************************************
                          gamewinsystem.h  -  Window managing with game key settings
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

#ifndef GAMEWINSYSTEM_H
#define GAMEWINSYSTEM_H

#include <vector> //STL vector template
namespace std {}
using namespace std;

#include "winsystem.h"
#include "cstring.h"

/**
  *@author CJP
  */

enum ePlayerKey
{
	eShiftUp=0,
	eShiftDown=1,
	eHorn=2,
	eCameraChange=3,
	eCameraToggle=4
};

enum ePlayerControl
{
	eUp=0,
	eDown=1,
	eLeft=2,
	eRight=3
};

enum eGlobalKey
{
	eNextSong=0,
	ePause=1
};

class CGameWinSystem : public CWinSystem  {
public: 
	CGameWinSystem(const CString &caption);
	virtual ~CGameWinSystem();

	virtual bool reloadConfiguration();

	float getPlayerControlState(ePlayerControl ctl, unsigned int player);
	bool getPlayerKeyState(ePlayerKey key, unsigned int player);
	bool playerKeyWasPressed(ePlayerKey key, unsigned int player);
	bool getGlobalKeyState(eGlobalKey key);
	bool globalKeyWasPressed(eGlobalKey key);

	//For use in e.g. the menu interface
	unsigned int getKeyFromGlobalKey(eGlobalKey key);

	CString key2name(unsigned int key) const;
	unsigned int name2key(const CString &name) const;

protected:
	/*
	Key codes are a general way to point to boolean input resources
	They have nothing implementation specific
	except that you should not rely on specific meanings of values
	*/

	unsigned int getGlobalKeyCode(eGlobalKey key);
	unsigned int getPlayerKeyCode(ePlayerKey key, unsigned int player);
	unsigned int getPlayerControlCode(ePlayerControl ctl, unsigned int player);

	bool getKeyCodeState(unsigned int code);
	bool keyCodeWasPressed(unsigned int code);
	float getControlCodeState(unsigned int code);

	//Overloaded: this one really implements it
	virtual unsigned int getJoystickKeyCode(unsigned int joystick, unsigned int button);
	unsigned int getJoystickAxisCode(unsigned int joystick, unsigned int axis, bool positive);
private:

	static void initKeyNameTranslationTable();
	struct SKeyName
	{
		SKeyName(int k, const CString &n)
			: key(k), name(n)
			{}

		int key;
		CString name;
	};
	static vector<SKeyName> m_KeyNames;

	//user-defined key codes
	typedef vector<unsigned int> tCodeArray;
	vector<tCodeArray> m_PlayerKeyCode;
	vector<tCodeArray> m_PlayerControlCode;
	tCodeArray m_GlobalKeyCode;

	void setupKeys(const CString &section, unsigned int player);
};

#endif
