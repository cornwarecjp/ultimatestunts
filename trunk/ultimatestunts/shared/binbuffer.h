/***************************************************************************
                          binbuffer.h  -  binary buffer class
                             -------------------
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

#ifndef _CBinBuffer_H_
#define _CBinBuffer_H_

#include <vector>
#include "cstring.h"
#include <SDL/SDL.h>
#include "usmacros.h"

using namespace std;

class CBinBuffer : public vector<Uint8>
{
  public:

	
   enum eError {
     eEndOfBuffer=1,
     eOtherError
   };


//	CBinBuffer & operator += (const char unsigned & c); //better use Uint8 ??
	CBinBuffer & operator += (const char unsigned * c);
	CBinBuffer & operator = (const char unsigned * c);
	CBinBuffer & operator += (const Uint8 & i);
	CBinBuffer & operator += (const Uint16 & i);
	CBinBuffer & operator += (const CBinBuffer & bb);
	CBinBuffer & operator += (const CString & bs);

   Uint8 getUint8(const int unsigned pos=0, int *newpos=NULL) const;
   Uint16 getUint16(const int unsigned pos=0, int *newpos=NULL) const;
   CString & getCString(const int unsigned pos=0, int *newpos=NULL) const;

   CString & dump() const;
   CBinBuffer & substr(const int unsigned pos=0, const int n=-1) const ;

   char * raw_str() const;
};

#endif	//_CBinBuffer_H_

