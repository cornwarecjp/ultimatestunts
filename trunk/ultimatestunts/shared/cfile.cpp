/***************************************************************************
                          cfile.cpp  -  CJP's file class
                             -------------------
    begin                : Thu May 23 2002
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
#include "cfile.h"
#include <cstdio>
#include <cstdlib>

CFile::CFile(CString filename, bool write)
{
	CFile::open(filename, write);
}

CFile::CFile()
{
	fp = NULL; //closed
}

CFile::~CFile()
{
	CFile::close();
}

bool CFile::open(CString filename, bool write)
{
	m_Filename = filename;
	m_Write = write;

	const char *fn = filename.c_str();

	if(write)
	{
		fp = fopen(fn,"wb");

		if(fp==NULL)
		{printf("Error: could not write to %s\n", fn); return false;}
	}
	else
	{
		fp = fopen(fn,"rb");

		if(fp==NULL)
			{printf("Error: could not read from %s\n", fn); return false;}
	}

	return true;
}

void CFile::close()
{
	if(fp==NULL) return;
	fclose(fp);
	fp = NULL;
}

void CFile::reopen()
{
	CFile::close();
	CFile::open(m_Filename, m_Write);
}

CString CFile::readl()
{
	if(fp==NULL)
		return "\n";

	CString ret;
	ret = "";

	int c = 0; //!='\n' en !=EOF
	while((c!='\n' && c!=EOF))
	{
		c = fgetc(fp);
		if (c=='\r') c = fgetc(fp); //MS-DOS indeling
		if(c!='\n' && c!=EOF)
			ret += c;
	}

	if(c==EOF && ret.length()==0)
		ret += '\n';

	return ret;
}

void CFile::writel(CString l)
{
  const char *c = l.c_str();
  fputs(c, fp);
  fputc('\n', fp);
}

CBinBuffer CFile::readBytes(unsigned int maxlen)
{
	if(fp==NULL)
		return CBinBuffer();

	Uint8 buffer[maxlen];

	unsigned int size = fread(buffer, 1, maxlen, fp);
	
	CBinBuffer ret;
	for(unsigned int i=0; i < size; i++)
		ret += buffer[i];

	return ret;
}

void CFile::writeBytes(const CBinBuffer &b)
{
	if(fp==NULL)
		return;

	Uint8 buffer[b.size()];
	for(unsigned int i=0; i < b.size(); i++)
		buffer[i] = b[i];

	fwrite(buffer, 1, b.size(), fp);
}
