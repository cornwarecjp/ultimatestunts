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

CUSServer::CUSServer(int port, CString trackfile)
{
	printf("Starting a server process @ port %d with track \"%s\"\n",
		port, trackfile.c_str());

	CString cmd = CString("stuntsserver")
		+ " port=" + port
		+ " > server.stdout 2> server.stderr";

	printf("Command is: \"%s\"\n", cmd.c_str());

	m_ServerInput = popen(cmd.c_str(), "w");

	//We don't want to use a super-server
	fprintf(m_ServerInput, "n\n");
	//Enter the track file:
	fprintf(m_ServerInput, "%s\n", trackfile.c_str());
	//Enter the number of remote players:
	fprintf(m_ServerInput, "%d\n", 1);
	//Enter the number of AI players:
	fprintf(m_ServerInput, "%d\n", 1);

	//Enter the AI player:
	fprintf(m_ServerInput, "AI Player on server\n");
	fprintf(m_ServerInput, "cars/porsche928.conf\n");
}

CUSServer::~CUSServer()
{
	pclose(m_ServerInput);
}
