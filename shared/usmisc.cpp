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

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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

#ifdef __APPLE__
// dynamic data path location on OS X

#include <CoreFoundation/CoreFoundation.h>

bool setPathIfInBundle(CString& returnpath)
{
	char path[1024];

	CFBundleRef mainBundle = CFBundleGetMainBundle();
	assert(mainBundle);

	CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
	assert(mainBundleURL);

	CFStringRef cfStringRef =
		CFURLCopyFileSystemPath(mainBundleURL, kCFURLPOSIXPathStyle);
	assert(cfStringRef);

	CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);

	CFRelease(mainBundleURL);
	CFRelease(cfStringRef);

	CString contents = CString(path) + "/Contents/Resources";
	if(contents.find(".app") != std::string::npos)
	{
		// executable is inside an app bundle, use app bundle-relative paths
		returnpath = contents;
		return true;
	}
	else
	{
		// executable is installed Unix-style, use default paths
		return false;
	}
}
#endif

#ifdef __CYGWIN__

CString getSystemLocale()
{
	CString ret;

	const char *cmd1 = "locale";
	const char *cmd2 = "-u";

	//Create a pipe
	int pipe_fds[2]; //[0] = reading [1] = writing
	if(pipe(pipe_fds) < 0)
		return ""; //failed
	int out = pipe_fds[1];
	int in  = pipe_fds[0];

	//Fork this process
	pid_t PID = fork(); //is vfork allowed?
	if(PID == 0)
	{
		//we are the child process

		close(in); //we don't need this side in this process

		//set the output to the pipe
		if(out != 1)
		{
			dup2(out, 1);
			close(out);
		}

		//Jump to the server binary
		execlp(cmd1, cmd1, cmd2, NULL);

		printf("Error: locale program could not be started\n");
		fflush(stdout);

		_exit(1); //failed to start the server process
	}

	//we are in the original process

	close(out); //we don't need this side in this process

	if(PID < 0) //fork failed
	{
		close(in);
		return "";
	}

	//Read from the pipe
	char c;
	while(read(in, &c, 1) == 1 && c != '\n')
		ret += c;

	close(in);

	//wait until the process stops
	int wstatus;
	int wait_pid = -1;
	do
	{
		wait_pid = waitpid(PID, &wstatus, 0);
	}
	while(wait_pid == -1 && errno == EINTR);

	return ret;
}

#endif

bool copyConfiguration(CString &conffile)
{
	printf("File %s not found. Searching on alternative locations...\n", conffile.c_str());

	CString sysconfdir = SYSCONFDIR;
	printf("sysconfdir = %s\n", sysconfdir.c_str());

	std::vector<CString> locations;
	locations.push_back(sysconfdir + "/ultimatestunts.conf");
	locations.push_back("/etc/ultimatestunts.conf");
	locations.push_back("/usr/local/etc/ultimatestunts.conf");
	locations.push_back("./ultimatestunts.conf");

#if defined(__APPLE__)
	// find config path dynamically
	CString osx_path;
	if(setPathIfInBundle(osx_path))
		locations.push_back(osx_path + "/etc/ultimatestunts.conf");
#endif

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
CString originalSystemLocale;

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
#ifdef __CYGWIN__
	originalSystemLocale = getSystemLocale();
#endif

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

#ifdef __APPLE__

	if(DataDir == "@appbundle/")
	{
		// find data path dynamically
		CString osx_path;
		if(setPathIfInBundle(osx_path))
		{
			DataDir = osx_path + "/data/";
		}
	}

#endif

	printf("DataDir is \"%s\"\n", DataDir.c_str());
	printf("SaveDir is \"%s\"\n", SaveDir.c_str());
	if(fctl == NULL) fctl = new CFileControl; //TODO: find a way to delete this object
	fctl->setDataDir(DataDir);
	fctl->setSaveDir(SaveDir);

	printf("Enabling localisation\n");

	//select a language
	CString conf_lang = theMainConfig->getValue("user_interface", "language");
	if(conf_lang == "system")
	{

#ifdef __CYGWIN__
		printf("System locale: %s\n", originalSystemLocale.c_str());
		char *retval = setlocale(LC_MESSAGES, originalSystemLocale.c_str());
#else
		char *retval = setlocale(LC_MESSAGES, "");
#endif

		if(retval == NULL)
		{
			printf("Setting the system LC_MESSAGES locale failed\n");
		}
		else
		{
			printf("Locale LC_MESSAGES is set to \"%s\"\n", retval);
		}

#ifdef __CYGWIN__
		//F* Cygwin doesn't set the locale correctly
		setenv("LC_MESSAGES", originalSystemLocale.c_str(), 1);
#endif

		setenv("LANGUAGE", originalLANGUAGEenv.c_str(), 1);
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

	//find the absolute path
	//very long paths might cause segfaults
	CString absdir = getAbsDir(DataDir);

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
