/***************************************************************************
                          main.cpp  -  Stunts-AI main
                             -------------------
    begin                : wo nov 20 23:23:28 CET 2002
    copyright            : (C) 2002 by CJP
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <cstdlib>

#include "hello.h"
#include "usmisc.h"

int main(int argc, char *argv[])
{
	printf("Welcome to " PACKAGE " version " VERSION "\n");

	shared_main(argc, argv);

	hello h;
	h.writeHello();

	return EXIT_SUCCESS;
}
