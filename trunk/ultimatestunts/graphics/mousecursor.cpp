/***************************************************************************
                          mousecursor.cpp  -  A mouse cursor texture
                             -------------------
    begin                : zo aug 26 2007
    copyright            : (C) 2007 by CJP
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
#include <GL/gl.h>

#include "mousecursor.h"

CMouseCursor::CMouseCursor() : CTexture(NULL)
{
	printf("Creating OpenGL mouse cursor\n");

	//sets m_Loaded -> texture unloads properly
	CDataObject::load("<MouseCursor>", CParamList());

	//Stolen from SDL documentation:
	const char *arrow = 
	"X                               "
	"XX                              "
	"X.X                             "
	"X..X                            "
	"X...X                           "
	"X....X                          "
	"X.....X                         "
	"X......X                        "
	"X.......X                       "
	"X........X                      "
	"X.....XXXXX                     "
	"X..X..X                         "
	"X.X X..X                        "
	"XX  X..X                        "
	"X    X..X                       "
	"     X..X                       "
	"      X..X                      "
	"      X..X                      "
	"       XX                       "
	"                                "
	"                                "
	"                                "
	"                                "
	"                                "
	"                                "
	"                                "
	"                                "
	"                                "
	"                                "
	"                                "
	"                                "
	"                                ";

	unsigned char data[4*32*32];

	{
		const char *in = arrow;
		unsigned char *out = data;
		for(unsigned int i=0; i < 32*32; i++)
		{
			switch(*in)
			{
			case '.':
				out[0] = out[1] = 0;
				out[2] = 255;
				out[3] = 192;
				break;
			case 'X':
				out[0] = out[1] = out[2] = 255;
				out[3] = 127;
				break;
			default:
				out[0] = out[1] = out[2] = out[3] = 0;
			}

			in++;
			out += 4;
		}
	}

	glGenTextures(1, &m_Texture);
	glBindTexture(GL_TEXTURE_2D, m_Texture);

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

	if(m_TextureSmooth)
	{
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
	else
	{
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	}

}


CMouseCursor::~CMouseCursor()
{
}


