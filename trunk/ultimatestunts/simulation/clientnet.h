/***************************************************************************
                          clientnet.h  -  Client-side network class
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

#ifndef CLIENTNET_H
#define CLIENTNET_H

#include "cstring.h"

/**
  *@author CJP
  */

class CClientNet { //TODO: derive from CFileCtrl (and CStuntsNet)
public: 
	CClientNet(CString host="localhost", int port=1500);
	~CClientNet();
};

#endif
