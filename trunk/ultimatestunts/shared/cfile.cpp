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
#include <stdio.h>
#include <stdlib.h>

CFile::CFile(CString filename, bool write)
{
	const char *fn = filename.c_str();

  if(write)
  {
  	fp = fopen(fn,"w");

	  if(fp==NULL)
		  printf("Error: could not write to %s\n", fn);
  }
  else
  {
  	fp = fopen(fn,"r");

	  if(fp==NULL)
		  printf("Error: could not read from %s\n", fn);
  }

}

CFile::~CFile()
{
	if(fp==NULL) return;
	fclose(fp);
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
