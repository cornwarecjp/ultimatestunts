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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <errno.h>

#include <cstdio>
#include <cstdlib>
 

#include "usserver.h"

CUSServer::CUSServer(int port, const CString &serverName)
{
	startServer(port, serverName);
}

CUSServer::~CUSServer()
{
	stopServer();
}

void CUSServer::addai(const CString &name, const CString &car)
{
	giveCmd(CString("addai ") +name + ", " + car);
}

void CUSServer::clearai()
{
	giveCmd("clearai");
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
	//printf("Giving a command: %s...\n", cmd.c_str());

	CString cmd2 = cmd + "\n";

	//while(true);

	write(m_ServerFD, cmd2.c_str(), cmd2.length());

	//printf("End of giveCmd(..)\n");
}

bool CUSServer::startServer(int port, const CString &serverName)
{
	printf("Starting a server process\n");

	CString c1 = "ustuntsserver";
	CString c2 = CString("port=") + port;
	CString c3 = CString("serverName=") + serverName;

	const char *cmd1 = c1.c_str();
	const char *cmd2 = c2.c_str();
	const char *cmd3 = c3.c_str();

	printf("Server command is: %s;%s;%s\n", cmd1, cmd2, cmd3);

	//Create a pipe
	int pipe_fds[2]; //[0] = reading [1] = writing
	if(pipe(pipe_fds) < 0)
		return false; //failed
	m_ServerFD = pipe_fds[1];
	int serverstdin = pipe_fds[0];

	//printf("Pipe is succesful: %d -> %d\n", m_ServerFD, serverstdin);

	//Fork this process
	m_ServerPID = fork(); //is vfork allowed?
	if(m_ServerPID == 0)
	{
		//we are the child process

		close(m_ServerFD); //we don't need this side in this process

		//set the stdin to the pipe
		if(serverstdin != 0)
		{
			dup2(serverstdin, 0);
			close(serverstdin);
		}

		//Set the stdout and stderr
		int outFD = creat("server-stdout.txt", S_IREAD | S_IWRITE);
		int errFD = creat("server-stderr.txt", S_IREAD | S_IWRITE);
		if(outFD < 0 || errFD < 0)
			_exit(2); //failed to open output files
		dup2(outFD, 1);
		dup2(errFD, 2);
		close(outFD);
		close(errFD);

		//printf("Ready to jump to the server binary...\n");
		//printf("Command is %s;%s;%s\n", cmd1, cmd2, cmd3);
		//fflush(stdout);

		//Jump to the server binary
		execlp(cmd1, cmd1, cmd2, cmd3, NULL);

		printf("Error: server could not be started!\n");
		fflush(stdout);

		_exit(1); //failed to start the server process
	}

	//we are in the original process


	//printf("Fork was succeful: PID = %d\n", m_ServerPID);

	close(serverstdin); //we don't need this side in this process

	if(m_ServerPID < 0) //fork failed
	{
		close(m_ServerFD);
		return false;
	}

	//printf("End of startServer(..)\n");
	return true;
}

bool CUSServer::stopServer()
{
	//Give the command to exit the server
	giveCmd("exit");

	//Close the pipe
	close(m_ServerFD);

	//wait until the process stops
	int wstatus;
	int wait_pid = -1;
	do
	{
		wait_pid = waitpid(m_ServerPID, &wstatus, 0);
	}
	while(wait_pid == -1 && errno == EINTR);

	if(wait_pid == -1) return false;

	return true;
}
