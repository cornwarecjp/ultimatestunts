/***************************************************************************
                          glextensions.h  -  Interface to various openGL extensions
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

#ifndef GLEXTENSIONS_H
#define GLEXTENSIONS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "usmacros.h"


#include <GL/gl.h>

#ifdef HAVE_GLEXT_H

#include <GL/glext.h>

#else

//dirty hack on systems that don't have glext.h:

#ifdef __CYGWIN__
#define APIENTRY __attribute__ ((__stdcall__))
#else
#define APIENTRY
#endif

typedef void (APIENTRY * PFNGLLOCKARRAYSEXTPROC) (GLint, GLsizei);
typedef void (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC) (void);

#undef APIENTRY

#endif

/**
  *@author CJP
  */

class CGLExtensions
{
public:
	CGLExtensions();
	~CGLExtensions();

	//GL_EXT_compiled_vertex_array
	static bool hasCompiledVertexArray;
	static PFNGLLOCKARRAYSEXTPROC glLockArrays;
	static PFNGLUNLOCKARRAYSEXTPROC glUnlockArrays;
};

#endif
