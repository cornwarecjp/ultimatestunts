/***************************************************************************
                          usserver.h  -  interface to a local server process
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

#ifndef USSERVER_H
#define USSERVER_H

#include <stdio.h> //for FILE
#include "cstring.h"

/**
  *@author CJP
  */

class CUSServer {
public: 
	CUSServer(int port, CString trackfile="tracks/default.track");
	~CUSServer();

protected:
	FILE *m_ServerInput;
};

#endif
