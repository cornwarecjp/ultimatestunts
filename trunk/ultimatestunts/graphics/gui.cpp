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
	return "";
}

void CGUI::resetSection(CString section)
{
	if(section=="") //reset all sections
		{m_PassedMainMenu = false; m_PassedHostMenu = false;}

	if(section=="mainmenu")
		m_PassedMainMenu = false;
	if(section=="hostmenu")
		m_PassedHostMenu = false;
}

bool CGUI::isPassed(CString section)
{
	if(section=="mainmenu")
		return m_PassedMainMenu;
	if(section=="hostmenu")
		return m_PassedHostMenu;

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
		int input;
		scanf("%d", &input);

		switch(input)
		{
			case 1:
				m_MainMenuInput = LocalGame;
				ret = ""; //TODO: track menu
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
	char input[80];
	scanf("%s", input);
	m_HostName = input;
	printf("Enter the server's UDP port number: ");
	scanf("%d", &m_HostPort);
	m_PassedHostMenu = true;

	return ""; //TODO: player menu
}
