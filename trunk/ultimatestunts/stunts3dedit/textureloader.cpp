/***************************************************************************
                          textureloader.cpp  -  description
                             -------------------
    begin                : do apr 10 2003
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

#include <cstdio>
#include "cfile.h"
#include "datafile.h"
#include "textureloader.h"

#include "lconfig.h"

CTextureLoader::CTextureLoader(CString texdatfile)
{
	//defaults:
	m_TexMaxSize = 1024;
	m_TexSmooth = true;

	const CLConfig &conf = *theMainConfig;

	CString cnf = conf.getValue("graphics", "texture_maxsize");
	if(cnf != "")
		m_TexMaxSize = cnf.toInt();

	cnf = conf.getValue("graphics", "texture_smooth");
	m_TexSmooth = (cnf != "false");

	CDataFile f(texdatfile);

	CString line = f.readl();
	if(!(line == "TEDITFILE"))
	{
		printf("Warning: incorrect textures.dat file\n");
	}

	while(true) //searching for BEGIN
	{
		line = f.readl();
		if(line[0] == '\n') return;

		if(line == "BEGIN")
			break;
	}

	line=f.readl();  //# of textures
	m_N = line.toInt();
	m_TexArray = new CLODTexture *[m_N];

	for(int i=0; i<m_N; i++) //textures
	{
		line = f.readl();
		m_TexArray[i] = new CLODTexture(NULL);

		printf("%s\n", line.c_str());
		CParamList plist;
		SParameter p;
		p.name = "sizex";
		p.value = m_TexMaxSize;
		plist.push_back(p);
		p.name = "sizey";
		p.value = m_TexMaxSize;
		plist.push_back(p);
		p.name = "smooth";
		p.value = m_TexSmooth;
		plist.push_back(p);
		m_TexArray[i]->load(line, plist);
	}

}

CTextureLoader::~CTextureLoader()
{
	for(int i=0; i<m_N; i++)
		delete m_TexArray[i];

	delete [] m_TexArray;
}
