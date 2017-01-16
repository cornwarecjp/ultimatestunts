/***************************************************************************
                          trkfile.cpp  -  An original Stunts track file
                             -------------------
    begin                : sa jul 29 2006
    copyright            : (C) 2006 by CJP
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

#include "trkfile.h"
#include "cfile.h"

#define TRKLENGTH 1802

CTRKFile::CTRKFile()
{
	vector<STile> tmp;
	tmp.resize(30);
	m_Track.resize(30);
	for(unsigned int i=0; i<30; i++)
		m_Track[i] = tmp;
}

CTRKFile::~CTRKFile()
{
}

bool CTRKFile::load(const CString &filename)
{
	CFile f(filename);

	CBinBuffer bytes = f.readBytes(TRKLENGTH);

	if(bytes.size() != TRKLENGTH)
	{
		printf("Expected %d bytes, got %lu bytes\n", TRKLENGTH, static_cast<unsigned long>(bytes.size()));
		return false;
	}

	if(f.readBytes(1).size() != 0)
	{
		printf("Didn't expect more than %d bytes\n", TRKLENGTH);
		return false;
	}

	//Read index
	unsigned int i = 0;

	//Items
	//for(unsigned int y=0; y<30; y++)
	for(int y=29; y>=0; y--)
	for(unsigned int x=0; x<30; x++)
	{
		m_Track[y][x].item = bytes[i];
		i++;
	}

	//Skybox
	m_Skybox = bytes[i];
	i++;

	//for(int y=29; y>=0; y--)
	for(unsigned int y=0; y<30; y++)
	for(unsigned int x=0; x<30; x++)
	{
		m_Track[y][x].terrain = bytes[i];
		i++;
	}

	/*
	printf("Skybox = %d\n", m_Skybox);
	for(unsigned int y=0; y<30; y++)
	{
		for(unsigned int x=0; x<30; x++)
		{
			printf("%2x:%2x ", m_Track[x][y].terrain, m_Track[x][y].item);
		}
		printf("\n");
	}
	*/

	return true;
}
