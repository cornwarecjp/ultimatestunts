/***************************************************************************
                          usmisc.cpp  -  Misc utility functions
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

#include <cstdio>
#include <cstdlib>

//internationalisation:
#include <locale.h>
#include <libintl.h>
#define _(String) gettext (String)

#include "usmisc.h"
#include "cstring.h"
#include "lconfig.h"
#include "filecontrol.h"


void shared_main(int argc, char *argv[])
{
	theMainConfig = new CLConfig(argc, argv);
	if(!theMainConfig->setFilename("ultimatestunts.conf"))
	{
		printf("Error: could not read ultimatestunts.conf\n");
		//TODO: create a default one
	}
	else
	{
		printf("Using ultimatestunts.conf\n");
	}

	//The data dir
	//Default:
	CString DataDir = ""; //try in default directories, like "./"

	CString cnf = theMainConfig->getValue("files", "datadir");
	if(cnf != "")
	{
		if(cnf[cnf.length()-1] != '/') cnf += '/';
		DataDir = cnf;
	}

	//find the absolute path
	//very long paths might cause segfaults
	char datadirbuffer[4096];
	if(realpath(DataDir.c_str(), datadirbuffer) != NULL)
	{
		DataDir = CString(datadirbuffer) + "/";
	}
	printf("DataDir is \"%s\"\n", DataDir.c_str());

	printf("Setting up the data dir\n");
	CFileControl *fctl = new CFileControl; //TODO: find a way to delete this object
	fctl->setDataDir(DataDir);

	printf("Enabling localisation\n");
	setlocale(LC_MESSAGES, "");
	bindtextdomain(PACKAGE, (DataDir + "lang").c_str());
	textdomain(PACKAGE);
}
