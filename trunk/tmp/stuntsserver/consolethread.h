/***************************************************************************
                          consolethread.h  -  The thread that does the stdio
                             -------------------
    begin                : do jan 20 2005
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

#ifndef CONSOLETHREAD_H
#define CONSOLETHREAD_H

/**
  *@author CJP
  */

#include "cstring.h"


//it does not need to be derived from CBThread,
//because this is actually the main thread
class CConsoleThread {
public: 
	CConsoleThread();
	~CConsoleThread();

	//use this to put a message on stdout:
	void write(const CString &s);

	//only to be called from the main thread:
	void startConsole();
	void cmd_start();
	void cmd_stop();
	void cmd_addai(const CString &args);
	void cmd_clearai();
	void cmd_set(const CString &args);
	void cmd_show();
	void cmd_online();
	void cmd_offline();
	void cmd_write(const CString &args);

	bool m_IsOnline;

private:
	CString getInput(CString question="");

	bool executeCommand(const CString &cmd);

	void updateMetaServerData();
};

#endif
