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

#include "config.h"

//internationalisation:
#include <locale.h>
#include <libintl.h>
#define _(String) gettext (String)

#include "usmisc.h"
#include "usmacros.h"

#include "cstring.h"
#include "lconfig.h"
#include "filecontrol.h"
#include "cfile.h"


bool copyConfiguration(CString &conffile)
{
	printf("File %s not found. Searching on alternative locations...\n", conffile.c_str());

	CString prefixdir = PREFIXDIR;
	printf("prefix = %s\n", prefixdir.c_str());

	std::vector<CString> locations;
	locations.push_back(prefixdir + "/etc/ultimatestunts.conf");
	locations.push_back("/etc/ultimatestunts.conf");
	locations.push_back("/usr/local/etc/ultimatestunts.conf");
	locations.push_back("./ultimatestunts.conf");

	CString sourceConffile;
	for(unsigned int i=0; i < locations.size(); i++)
	{
		CString &loc = locations[i];

		printf("Trying %s...", loc.c_str());
		if(fileExists(loc))
		{
			printf("found\n");
			sourceConffile = loc;
			break;
		}
		else
		{
			printf("not found\n");
		}
	}

	if(sourceConffile == "")
	{
		printf("WARNING: no configuration file found!\n");
		return false;
	}

	printf("Copying %s to %s\n", sourceConffile.c_str(), conffile.c_str());
	if(!copyFile(sourceConffile, conffile))
	{
		printf("  copying FAILED! (maybe we don't have the right permissions)\n");
		return false;
	}

	return true;
}

//exceptions for the development environment:
bool inDevelopmentTree()
{
	return fileExists("./execselect.sh");
}

CFileControl *fctl = NULL;
CString originalLANGUAGEenv;

void shared_main(int argc, char *argv[])
{
	CString conffile;

	//First process some commandline arguments
	for(int i=0; i < argc; i++)
	{
		CString arg = argv[i];

		if(arg == "--help")
		{
			printf(
			"Usage: %s [--help] [--conf=conffile] [SECTION:OPTION=VALUE]...\n"
			"Starts the Ultimate Stunts program %s\n"
			"\n"
			"  --help                Show this help and exit\n"
			"  --conf=conffile       Load settings from conffile\n"
			"  SECTION:OPTION=VALUE  Override a conffile setting (NYI!)\n"
			"\n",
			argv[0], argv[0]
			);
			exit(EXIT_SUCCESS);
		}

		if(arg.mid(0, 7) == "--conf=")
		{
			conffile = arg.mid(7);
		}
	}


#ifdef UNIX_TREE
	CString homedir = getenv("HOME");
#endif

	{
		char *env = getenv("LANGUAGE");
		if(env != NULL)
			originalLANGUAGEenv = env;
	}

	//Find the conf file location, if not given on commandline
	if(conffile=="")
	{
		//the default directory of the configuration file
		CString confdir;
#ifdef UNIX_TREE
		confdir = homedir + "/.ultimatestunts/";
#else
		confdir = "./";
#endif

		if(inDevelopmentTree())
		{
			printf(
				"execselect.sh detected:\n"
				"  We are probably in the Ultimate Stunts SOURCE TREE\n"
				"EXECUTED FROM THE SOURCE TREE:\n"
				"  Using the conf file in the source tree\n"
				);
			confdir = "./";
		}

		//make sure that the directory exists
		makeDir(confdir);

		//make sure that the conf file exists
		conffile = confdir + "ultimatestunts.conf";
		if(!fileExists(conffile))
			copyConfiguration(conffile);
	}
	
	printf("Using configuration file %s\n", conffile.c_str());

	theMainConfig = new CLConfig(argc, argv);
	if(!theMainConfig->setFilename(conffile))
	{
		printf("Error: could not read configuration file\n");
		//TODO: create a default one
	}

	update_shared_configuration();
}

void update_shared_configuration()
{
	//The data dir and the save dir
	CString DataDir, SaveDir;

	CString cnf = theMainConfig->getValue("files", "datadir");
	if(cnf != "")
	{
		if(cnf[cnf.length()-1] != '/') cnf += '/';
		DataDir = cnf;
	}

	cnf = theMainConfig->getValue("files", "savedir");
	if(cnf != "")
	{
		if(cnf[cnf.length()-1] != '/') cnf += '/';
		SaveDir = cnf;
	}

#ifdef UNIX_TREE
	//fill in home dir for "~/"
	CString homedir = getenv("HOME");
	if(DataDir.mid(0, 2) == "~/")
		DataDir = homedir + DataDir.mid(1);
	if(SaveDir.mid(0, 2) == "~/")
		SaveDir = homedir + SaveDir.mid(1);
#endif

	if(inDevelopmentTree())
	{
		printf("EXECUTED FROM THE SOURCE TREE:\n  Using the data dirs in the source tree\n");
		DataDir = "./data/";
		SaveDir = "./saveddata/";
	}

	//find the absolute path
	//very long paths might cause segfaults
	CString absdir;
	char datadirbuffer[4096];
	if(realpath(DataDir.c_str(), datadirbuffer) != NULL)
	{
		absdir = CString(datadirbuffer) + "/";
	}
	printf("DataDir is \"%s\"\n", DataDir.c_str());
	printf("SaveDir is \"%s\"\n", SaveDir.c_str());
	if(fctl == NULL) fctl = new CFileControl; //TODO: find a way to delete this object
	fctl->setDataDir(DataDir);
	fctl->setSaveDir(SaveDir);

	printf("Enabling localisation\n");

	//select a language
	CString conf_lang = theMainConfig->getValue("misc", "language");
	if(conf_lang == "system")
	{
		char *retval = setlocale(LC_MESSAGES, "");
		if(retval == NULL)
		{
			printf("Setting the LC_MESSAGES locale to \"\" failed\n");
		}
		else
		{
			printf("Locale LC_MESSAGES is set to \"%s\"\n", retval);
		}

		if(originalLANGUAGEenv != "")
		{
			setenv("LANGUAGE", originalLANGUAGEenv.c_str(), 1);
		}
	}
	else
	{
		char *retval = setlocale(LC_MESSAGES, conf_lang.c_str());
		if(retval == NULL)
		{
			printf("Setting the LC_MESSAGES locale to \"%s\" failed\n", conf_lang.c_str());
		}
		else
		{
			printf("Locale LC_MESSAGES is set to \"%s\"\n", retval);
		}

#ifdef __CYGWIN__
		//F* Cygwin doesn't set the locale correctly
		setenv("LC_MESSAGES", conf_lang.c_str(), 1);
#endif

		setenv("LANGUAGE", conf_lang.c_str(), 1);
	}

	//select the Ultimate Stunts domain
	//We can only use ISO 8859-1 because of the font texture
	printf("  Package %s, directory %s\n", PACKAGE, (absdir + "lang").c_str());
	printf("  bindtextdomain returns %s\n",
	    bindtextdomain(PACKAGE, (absdir + "lang").c_str())
	    );
	printf("  bind_textdomain_codeset returns %s\n",
	    bind_textdomain_codeset(PACKAGE, "ISO-8859-1")
	    );
	printf("  textdomain returns %s\n",
	    textdomain(PACKAGE)
	    );
}

vector<CString> getCredits()
{
#include "credits.h"
}
