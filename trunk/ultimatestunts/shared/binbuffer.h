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

class CBinBuffer : public vector<Uint8>
{
  private:
    Uint16 m_newCursor;	

  public:

	
   enum eError {
     eEndOfBuffer=1,
     eOtherError
   };

   Uint16 getNewCursor() const { return (m_newCursor); }


	 CBinBuffer & operator += (const char unsigned & c) {
		this->push_back((Uint8) c); 	 
		return (*this);
	 }

   CBinBuffer & operator += (const Uint16 & i) {
     this->push_back(HIBYTE(i));
     this->push_back(LOBYTE(i));
     return (*this);
   }

   CBinBuffer & operator += (const CBinBuffer & bb) {
     for (int unsigned i=0;i<bb.size();i++) this->push_back(bb[i]);   // FIXME: slow
     return (*this);
   }

   CBinBuffer & operator += (const CString & bs) {
     this->push_back((Uint8) bs.size());
     for (int i=0;i<bs.size();i++) {
       this->push_back((Uint8) bs[i]);
     }
   }

   Uint8 getUint8(const int pos=0)  {
     if (pos > this->size()-1) throw(eEndOfBuffer);
     m_newCursor = pos+1;
     return ((Uint8) (*this)[pos]);
   }

   Uint16 getUint16(const int pos=0) {
     if (pos > this->size()-2) throw(eEndOfBuffer);
     m_newCursor = pos+2;
     return (((*this)[pos] << 8) + (*this)[pos+1]);
   }

   CString & getCString(const int pos=0) {
     CString *res = new CString();
     if (pos > this->size()-1) throw(eEndOfBuffer);
     Uint8 ssize = (*this)[pos];
     if ((pos + ssize) > this->size()-1) throw(eEndOfBuffer);
     for (int i=0;i<ssize;i++) {
       res->push_back((*this)[pos+1+i]);
     }
     m_newCursor = pos+1+ssize;
     return (*res);
   }
   	
   CString & dump() const {
     CString *res = new CString();
     for (int unsigned i=0;i<this->size();i++) {
       char c=(*this)[i];
       char s[10];
       if ( ((int (c) >= 97) && (int (c) <= 122)) ||
            ((int (c) >= 65) && (int (c) <= 90))
          )
       {
          sprintf(s, "('%c' 0x%x)", c, (Uint8) c);
       } else {
          sprintf(s, "('?' 0x%x)", (Uint8) c);
       }
       *res+=s;
     }
     return (*res);
   }

   CBinBuffer & substr(const int unsigned pos = 0, const int n = -1) const {
    CBinBuffer *res = new CBinBuffer();
    if (pos <= this->size()) {
       int len = n;
       if (len == -1) { len = this->size() -pos; }
       for (int i = pos;i<len+pos;i++) {
         res->push_back((*this)[i]);
       }
    }
    return (*res);
   }

	
};


#endif	//_CBinBuffer_H_

