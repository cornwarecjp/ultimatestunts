/***************************************************************************
                          usmisc.h  -  Misc utility functions
                             -------------------
    begin                : wo feb 2 2005
    copyright            : (C) 2005 by CJP
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
#ifndef USMISC_H
#define USMISC_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <vector>
namespace std {}
using namespace std;

#include "cstring.h"

/*
this should be the start of all Ultimate Stunts
programs. It finds and loads the configuration file,
sets the datadir up, and initialises internationalisation.
*/
void shared_main(int argc, char *argv[]);

/*
Call whenever changes to the configuration options are made
*/
void update_shared_configuration();

bool inDevelopmentTree();

vector<CString> getCredits();

#endif

