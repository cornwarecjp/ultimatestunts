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
#include <dirent.h>
#include <sys/stat.h>

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

bool fileExists(const CString &filename)
{
	//try to open it with fopen
	FILE *fp = fopen(filename.c_str(), "r");

	if(fp == NULL) return false;

	fclose(fp);
	return true;
}

bool dirExists(const CString &dirname)
{
	DIR *theDir = opendir(dirname.c_str());

	if(theDir == NULL) return false;

	closedir(theDir);
	return true;
}

vector<CString> getDirContents(const CString &dir, const CString &ext)
{
	vector<CString> ret;

	DIR *dir1 = opendir(dir.c_str());

	if(dir1 != NULL)
	{
		while(true)
		{
			struct dirent *entry = readdir(dir1);
			if(entry == NULL) break;

			CString entname = entry->d_name;
			CString ent_lcase = entname;
			ent_lcase.toLower();
			//file extension check:
			if(ext == "" ||
				(ent_lcase.inStr(ext) >= 0 &&
				ent_lcase.inStr(ext) == (int)(ent_lcase.length() - ext.length()) )
				)
				ret.push_back(entname);
		}

		closedir(dir1);
	}

	return ret;
}

bool makeDir(const CString &dirname)
{
	//printf("makeDir(\"%s\");\n", dirname.c_str());
	if(!dirExists(dirname))
	{
		int i = 1;

		while(true)
		{
			int nextSlash = dirname.mid(i+1).inStr('/');

			CString subdir;
			if(nextSlash < 0)
			{
				nextSlash = dirname.length()-1;
				subdir = dirname;
			}
			else
			{
				nextSlash += i+1;
				subdir = dirname.mid(0, nextSlash);
			}

			if(!dirExists(subdir))
			{
				printf("Making dir %s\n", subdir.c_str());
				mkdir(subdir.c_str(), 00700);
			}

			i = nextSlash;
			if(i == int(dirname.length())-1) break;
		}
	}

	if(!dirExists(dirname)) return false;

	return true;
}

bool makeFile(const CString &filename)
{
	//find the last slash
	int pos = 0;
	while(true)
	{
		int nextSlash = filename.mid(pos+1).inStr('/');
		if(nextSlash < 0) break;

		nextSlash += pos+1;

		pos = nextSlash;
	}

	if(!makeDir(filename.mid(0, pos)) ) return false;

	FILE *fp = fopen(filename.c_str(), "w");
	if(fp == NULL) return false;
	fclose(fp);

	return true;
}

bool copyFile(const CString &src, const CString &dest)
{
	FILE *fp1 = fopen(src.c_str(), "r");
	if(fp1 == NULL) return false;

	FILE *fp2 = fopen(dest.c_str(), "w");
	if(fp2 == NULL)
	{
		fclose(fp1);
		return false;
	}

	while(true)
	{
		int c = fgetc(fp1);
		if(c == EOF) break;
		fputc(c, fp2);
	}

	fclose(fp1);
	fclose(fp2);
	return true;
}
