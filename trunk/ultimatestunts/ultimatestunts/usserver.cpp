/***************************************************************************
                          usserver.cpp  -  interface to a local server process
                             -------------------
    begin                : do mei 1 2003
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

#include "usserver.h"

CUSServer::CUSServer(int port, int maxRequests)
{
	printf("Starting a server process\n");

	CString cmd = CString("stuntsserver")
		+ " port=" + port
		+ " maxRequests=" + maxRequests
		+ " > server-stdout.txt 2> server-stderr.txt";

	printf("Command is: \"%s\"\n", cmd.c_str());

	m_ServerInput = popen(cmd.c_str(), "w");
}

CUSServer::~CUSServer()
{
	giveCmd("exit");
	pclose(m_ServerInput);
}

void CUSServer::addai(const CString &name, const CString &car)
{
	giveCmd(CString("addai ") +name + ", " + car);
}

void CUSServer::set(const CString &var, const CString &val)
{
	giveCmd(CString("set ") + var + " = " + val);
}

void CUSServer::start()
{
	giveCmd("start");
}

void CUSServer::stop()
{
	giveCmd("stop");
}

void CUSServer::giveCmd(const CString &cmd)
{
	fprintf(m_ServerInput, "%s\n", cmd.c_str());
	fflush(m_ServerInput);
}
