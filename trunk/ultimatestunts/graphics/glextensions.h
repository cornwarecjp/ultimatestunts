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

#include <GL/gl.h>
#include <GL/glext.h>

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
