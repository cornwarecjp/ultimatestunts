/***************************************************************************
                          glextensions.cpp  -  Interface to various openGL extensions
                             -------------------
    begin                : do feb 3 2005
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
#include <cstring>

#include "SDL.h"

#include "lconfig.h"
#include "glextensions.h"

bool _GLExtensionsScanned = false;

bool CGLExtensions::hasCompiledVertexArray;
PFNGLLOCKARRAYSEXTPROC CGLExtensions::glLockArrays;
PFNGLUNLOCKARRAYSEXTPROC CGLExtensions::glUnlockArrays;

CGLExtensions::CGLExtensions()
{
	if(_GLExtensionsScanned) return; //the variables are already initialised

	/*
	const char *extensions = (const char *)glGetString(GL_EXTENSIONS);


	//GL_EXT_compiled_vertex_array:
	if(strstr(extensions, "GL_EXT_compiled_vertex_array") != NULL)
		printf("Compiled vertex array extension found in extension string\n");
	*/

	glLockArrays = (PFNGLLOCKARRAYSEXTPROC)SDL_GL_GetProcAddress("glLockArraysEXT");
	glUnlockArrays = (PFNGLUNLOCKARRAYSEXTPROC)SDL_GL_GetProcAddress("glUnlockArraysEXT");
	if(glLockArrays == NULL || glUnlockArrays == NULL)
	{
		hasCompiledVertexArray = false;
		printf("Compiled vertex array functions NOT found in openGL driver\n");
	}
	else
	{
		printf("Compiled vertex array functions found in openGL driver\n");

		if(theMainConfig->getValue("graphics", "compiledvertexarray") == "false")
		{
			printf("...NOT using it because this option is disabled\n");
			hasCompiledVertexArray = false;
		}
		else
		{
			printf("..and we'll be using it!\n");
			hasCompiledVertexArray = true;
		}
	}

	_GLExtensionsScanned = true;
}

CGLExtensions::~CGLExtensions()
{
}
