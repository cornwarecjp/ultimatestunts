/***************************************************************************
                          gui.cpp  -  The graphical user interface: menu's etc.
                             -------------------
    begin                : vr jan 31 2003
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

#include <cstdio>

#include "gui.h"
#include "usmacros.h"
#include "cfile.h"

CGUI::CGUI(const CLConfig &conf, CWinSystem *winsys)
{
	//TODO: get conffile options

	m_WinSys = winsys;

	//Default: not passed
	resetSection();

}

CGUI::~CGUI(){
}


void CGUI::startFrom(CString section)
{
	printf(
		"\n\n\n\n**********\n"
		"*The \"GUI\"*\n"
		"**********\n"
		"This is a temporary implementation of the GUI,\n"
		"based on stdio. It is not yet multithreaded.\n"
		"So you'll first have to answer all questions before\n"
		"we can continue.\n"
		);

	m_Stop = false;
	while(!m_Stop && section != "")
	{
		printf("\n\n"); //create some space

		if(section=="mainmenu")
			section = viewMainMenu();
		else if(section=="hostmenu")
			section = viewHostMenu();
		else if(section=="servermenu")
			section = viewServerMenu();
		else if(section=="trackmenu")
			section = viewTrackMenu();
		else if(section=="playermenu")
			section = viewPlayerMenu();
		else
			{printf("Error: unknown menu %s\n", section.c_str()); section = "";}
	}

	printf(
		"\n\n*****************\n"
		"*End of the \"GUI\"*\n"
		"*****************\n\n\n");
}

void CGUI::stop()
{
	//Has no meaning when not using multithreading
	m_Stop = true;
}

CString CGUI::getValue(CString section, CString field)
{
	if(section=="mainmenu")
		if(field=="") return m_MainMenuInput;

	if(section=="hostmenu")
	{
		if(field=="hostname") return m_HostName;
		if(field=="portnumber") return m_HostPort;
	}
	if(section=="servermenu")
	{
		if(field=="portnumber") return m_ServerPort;
	}
	if(section=="trackmenu")
		if(field=="") return m_TrackFile;

	if(section=="playermenu")
	{
		if(field=="number") return (int)(m_PlayerDescr.size());
		int sp = field.inStr(' ');
		if(sp > 0)
		{
			CString lhs = field.mid(0, sp);
			int rhs = field.mid(sp+1).toInt();
			if(lhs == "name")
				return m_PlayerDescr[rhs].name;
			if(lhs == "ishuman")
			{
				if(m_PlayerDescr[rhs].isHuman)
					{return "true";}
				else
					{return "false";}
			}
			if(lhs == "carfile")
				return m_PlayerDescr[rhs].carFile;
		}
	}
	return "";
}

void CGUI::resetSection(CString section)
{
	if(section=="") //reset all sections
	{
		m_PassedMainMenu = false;
		m_PassedHostMenu = false;
		m_PassedServerMenu = false;
		m_PassedTrackMenu = false;
		m_PassedPlayerMenu = false;
	}

	if(section=="mainmenu")
		m_PassedMainMenu = false;
	if(section=="hostmenu")
		m_PassedHostMenu = false;
	if(section=="servermenu")
		m_PassedServerMenu = false;
	if(section=="trackmenu")
		m_PassedTrackMenu = false;
	if(section=="player")
		m_PassedPlayerMenu = false;
}

bool CGUI::isPassed(CString section)
{
	if(section=="mainmenu")
		return m_PassedMainMenu;
	if(section=="hostmenu")
		return m_PassedHostMenu;
	if(section=="servermenu")
		return m_PassedServerMenu;
	if(section=="trackmenu")
		return m_PassedTrackMenu;
	if(section=="playermenu")
		return m_PassedPlayerMenu;

	return false; //all unknown sections are not passed
}

CString CGUI::viewMainMenu()
{
	CString ret = "incorrect_answer";
	while(ret == "incorrect_answer")
	{
		printf(
			"Main menu:\n"
			"  1: Play a local game\n"
			"  2: Start a new network game\n"
			"  3: Log in on a remote network game\n"
			"  \n"
			"  4: Exit\n"
		);
		int input = getInput().toInt();

		switch(input)
		{
			case 1:
				m_MainMenuInput = LocalGame;
				ret = "trackmenu"; //TODO: track menu
				break;
			case 2:
				m_MainMenuInput = NewNetwork;
				ret = "servermenu";
				break;
			case 3:
				m_MainMenuInput = JoinNetwork;
				ret = "hostmenu";
				break;
			case 4:
				m_MainMenuInput = Exit;
				ret = "";
				break;
			default:
				printf("Incorrect answer\n");
		}

	} //while

	m_PassedMainMenu = true;
	return ret;
}

CString CGUI::viewServerMenu()
{
	printf("Server menu:\n");
	printf("Enter the port number: ");
	m_ServerPort = getInput().toInt();

	m_PassedServerMenu = true;
	return "trackmenu";
}

CString CGUI::viewHostMenu()
{
	printf(
		"Host menu:\n"
		"Enter the server's host name: "
	);
	m_HostName = getInput();
	printf("Enter the server's UDP port number: ");
	m_HostPort = getInput().toInt();

	m_PassedHostMenu = true;
	return "playermenu";
}

CString CGUI::viewTrackMenu()
{
	CString defaulttrack = "tracks/default.track";
	printf("Track menu:\n");
	printf("Enter the track filename (default is %s): ", defaulttrack.c_str());
	m_TrackFile = getInput();
	if(m_TrackFile == "") m_TrackFile = defaulttrack;

	m_PassedTrackMenu = true;
	return "playermenu";
}

CString CGUI::viewPlayerMenu()
{
	CString defaultcar = "cars/diablo.conf";
	m_PlayerDescr.clear();

	printf("Player menu:\n");
	printf("Enter your name: ");
	CString name = getInput();
	printf("Enter your car file (default is %s):", defaultcar.c_str());
	CString carfile = getInput();
	if(carfile == "") carfile = defaultcar;
	addPlayer(name, true, carfile);

	while(true)
	{
		printf("\nDo you want to add a player(y/n)? ");
		CString answ = getInput();
		if(!(answ == "y" || answ == "Y")) break;

		printf("Enter the name: ");
		name = getInput();
		printf("Enter the car file (default is %s):", defaultcar.c_str());
		carfile = getInput();
		if(carfile == "") carfile = defaultcar;
		printf("Is it an AI player(y/n)? ");
		answ = getInput();
		bool isHuman = !(answ == "y" || answ == "Y");
		addPlayer(name, isHuman, carfile);
	}

	m_PassedPlayerMenu = true;
	return ""; //TODO: loading
}

void CGUI::addPlayer(CString name, bool human, CString carfile)
{
	SPlayerDescr pd;
	pd.name = name;
	pd.isHuman = human;
	pd.carFile = carfile;
	m_PlayerDescr.push_back(pd);
}

CString CGUI::getInput()
{
	char input[80];
	fgets(input, 79, stdin);
	CString ret(input);
	ret = ret.mid(0, ret.length() - 1); //remove \n
	return ret;
}
