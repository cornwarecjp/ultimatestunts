/***************************************************************************
                          CString.h  -  bones very own easy string class ;-)
                             -------------------
    begin                : Mon Sep 2 2002
    copyright            : (C) 2002 by bones
    email                : boesemar@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _CString_H
#define _CString_H


// #include <std/bastring.h>
#include <string>

#include "vector.h"

//  class CString : public std::basic_string<char>
 class CString : public std::string
 {
  private:
    char c_up(const char c) const;
    char c_down(const char c) const;

 public:
	//Constructors:
   CString(const char *s);
   CString();
	CString(const CVector &v);

	//Bones functions:
   CString & trimL();
   CString & trimR();
   CString & Trim();

   CString & toUpper();
   CString & toLower();
   CString & subStr(const int unsigned pos = 0, const int n = -1) const;
   CString & operator= (const char *str);
   CString getFirstLine();
   int countChar(const char c) const;
   int toInt();
   CString & dump() const;

	//CJP functions:
	CString(int val);
	CString(float val);
	CString operator+(CString const &val); //std didn't do what I wanted
	CString operator+(const char *val);
	bool operator!= (const char *str) const;
	bool operator!= (const CString &val) const;

	//Don't know how to do these with std::string:
	CString mid(unsigned int i, int l=-1) const; //-1 means as long as possible
	int inStr(char c) const;
	int inStr(const CString &s) const;

	//Don't know how to do this with std::string:
	float toFloat();

	//String->vector conversion:
	CVector toVector();
};

CString operator+(const char *val1, CString const &val2);

#endif

