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
	m_PassedMainMenu = false;
	m_PassedHostMenu = false;
}

CGUI::~CGUI(){
}


void CGUI::startFrom(eMenu menu)
{
	printf(
		"This is a temporary implementation of the GUI,\n"
		"based on stdio. It is not yet multithreaded.\n"
		"So you'll first have to answer all questions before\n"
		"we can continue.\n"
		);

	m_Stop = false;
	while(!m_Stop && menu != NoMenu)
	{
		printf("\n\n"); //create some space
		switch(menu)
		{

			case MainMenu:
				menu = viewMainMenu();
				m_PassedMainMenu = true;
				break;
			case HostMenu:
				menu = viewHostMenu();
				m_PassedHostMenu = true;
				break;
			default:
				printf("Error: using an unimplemented menu.\n");
				m_Stop = true;
		}
	}
}

void CGUI::stop()
{
	//Has no meaning when not using multithreading
	m_Stop = true;
}

const void *CGUI::getData(eMenu menu, CString item)
{
	switch(menu)
	{
		case MainMenu:
			if(!m_PassedMainMenu) return NULL;
			//m_PassedMainMenu = false;
			return (void *)&m_MainMenuInput;
		case HostMenu:
			if(!m_PassedHostMenu) return NULL;
			if(item=="hostname")
				return (void *)&m_HostName;
			if(item=="portnumber")
				return (void *)&m_HostPort;
			//unknown item:
			printf("Error: host menu does not contain item \"%s\"\n", item.c_str());
			return NULL;
		default:
			printf("Error: asking for an unimplemented menu.\n");
			return NULL;
	}
}

CGUI::eMenu CGUI::viewMainMenu()
{
	while(true) //while we're giving an incorrect answer
	{
		printf(
			"Main menu:\n"
			"  1: Play a local game\n"
			"  2: Log in on a remote game\n"
			"  \n"
			"  3: Exit\n"
		);
		scanf("%d", &m_MainMenuInput);
		switch(m_MainMenuInput)
		{
			case 1:
				return TrackMenu;
			case 2:
				return HostMenu;
			case 3:
				return NoMenu;
		}

		printf("Incorrect answer\n");
		//and loop again
	}

	return NoMenu; //we'll never reach this code
}

CGUI::eMenu CGUI::viewHostMenu()
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

	return PlayerMenu;
}
