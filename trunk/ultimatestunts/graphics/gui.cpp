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

#include <stdio.h>

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
		else if(section=="playermenu")
			section = viewPlayerMenu();
		else
			{printf("Error: unknown menu %s\n", section.c_str()); section = "";}
	}
}

void CGUI::stop()
{
	//Has no meaning when not using multithreading
	m_Stop = true;
}

CString CGUI::getValue(CString section, CString field)
{
	if(section=="mainmenu")
	{
		if(field=="choice") return m_MainMenuInput;
	}
	if(section=="hostmenu")
	{
		if(field=="hostname") return m_HostName;
		if(field=="portnumber") return m_HostPort;
	}
	if(section=="playermenu")
	{
		if(field=="number") return (int)(m_PlayerDescr.size());
		int i = field.toInt();
		return m_PlayerDescr[i];
	}
	return "";
}

void CGUI::resetSection(CString section)
{
	if(section=="") //reset all sections
	{
		m_PassedMainMenu = false;
		m_PassedHostMenu = false;
		m_PassedPlayerMenu = false;
	}

	if(section=="mainmenu")
		m_PassedMainMenu = false;
	if(section=="hostmenu")
		m_PassedHostMenu = false;
	if(section=="player")
		m_PassedPlayerMenu = false;
}

bool CGUI::isPassed(CString section)
{
	if(section=="mainmenu")
		return m_PassedMainMenu;
	if(section=="hostmenu")
		return m_PassedHostMenu;
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
			"  2: Log in on a remote game\n"
			"  \n"
			"  3: Exit\n"
		);
		int input = getInput().toInt();

		switch(input)
		{
			case 1:
				m_MainMenuInput = LocalGame;
				ret = "playermenu"; //TODO: track menu
				break;
			case 2:
				m_MainMenuInput = JoinNetwork;
				ret = "hostmenu";
				break;
			case 3:
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

CString CGUI::viewPlayerMenu()
{
	m_PlayerDescr.clear();

	printf("Player menu:\n");
	printf("Enter your name: ");
	addPlayer(getInput(), true);

	while(true)
	{
		printf("\nDo you want to add an AI player(y/n)? ");
		CString answ = getInput();
		if(!(answ == "y" || answ == "Y")) break;

		printf("Enter the name: ");
		addPlayer(getInput(), false);
	}

	m_PassedPlayerMenu = true;

	return ""; //TODO: loading
}

void CGUI::addPlayer(CString name, bool human)
{
	CString pd = name;
	if(human)
		pd = "H" + pd;
	else
		pd = "A" + pd;

	m_PlayerDescr.push_back(pd);
}

CString CGUI::getInput()
{
	char input[80];
	scanf("%s", input);
	return input;
}
