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


#include <std/bastring.h>
#include <stdio.h>
#include <stdlib.h>

 using namespace std;

 class CString : public std::basic_string<char>
 {
  private:
    char c_up(const char c) const
    {
      if ((int (c) >= 97) && (int (c) <= 122)) {
       return ((char) (  ( (int) c) - 32));
      }
      return (c);
    }

    char c_down(const char c) const
    {
      if ((int (c) >= 65) && (int (c) <= 90)) {
       return ((char) (  ( (int) c) + 32));
      }
      return (c);


      return (c);
    }

 public:

   CString(const char *s) {
     this->assign(s);
   }

   CString() {
     this->assign("");
   };


   //
   // LTrim: Trim left
   //
   CString & trimL() {
     while (this->length() > 0)
     {
       bool e = false;
       switch ((char) (*this)[0]) {
         case '\n':
         case (char) 1:
         case '\r':
         case ' ': this->erase(0,1);
                   break;
         default: e = true;

       }
       if (e) break;
     }
     return (*this);
   }

   //
   // RTrim: Trim right
   //
   CString & trimR() {
     while (this->length() > 0)
     {
       bool e = false;
       switch ((char) (*this)[this->length()-1]) {
         case '\n':
         case (char) 1:
         case '\r':
         case ' ': this->erase(this->length()-1,1);
                   break;
         default: e = true;

       }
       if (e) break;
     }
     return (*this);
   }

   //
   //  Trim: Trim string
   //
   CString & Trim()
   {
      trimR();
      trimL();
      return (*this);
   }

   CString & toUpper() {
     for (int unsigned i=0;i<this->size();i++) {
       (*this)[i] = c_up((*this)[i]);
     }
     return *this;
   }

   CString & toLower() {
     for (int unsigned i=0;i<this->size();i++) {
       (*this)[i] = c_down((*this)[i]);
     }
     return *this;
   }

   CString & subStr(const int unsigned pos = 0, const int n = -1) const {
    CString *res = new CString("");
    if (pos <= this->length()) {
       int len = n;
       if (len == -1) { len = this->size() -pos; }
       res->assign(*this, pos, len);
    }
    return (*res);
   }

   CString & operator= (const char *str)
   {
      this->assign(str);
      return *this;
   }

   CString getFirstLine() {
     CString *res = new CString();
     for (int unsigned i=0; i < this->length(); i++) {
        if (((*this)[i] == '\n') || ((*this)[i] == '\r')) {
//           for (;(((*this)[i+1] == '\n') || ((*this)[i+1] == '\r'));i++);
           if (i<this->length()-1) { if (((*this)[i+1] == '\n') || ((*this)[i+1] == '\r')) i++; }

           res->assign(this->subStr(0,i+1));
//           res->Trim();
           (*this) = this->subStr(i+1);
//           this->LTrim();
           return (*res);
        }
     }
     return ("");
   }

   int countChar(const char c) const {
     int res = 0;
     for (int unsigned i=0;i<this->length(); i++) {
       if ((*this)[i] == c) res++;
     }
     return  (res);
   }

   int toInt() {
     return (atol(this->c_str()));
   }

   CString & dump() const {
     CString *res = new CString();
     for (int unsigned i=0;i<this->length();i++) {
       char c=(*this)[i];
       char s[10];
       if ( ((int (c) >= 97) && (int (c) <= 122)) ||
            ((int (c) >= 65) && (int (c) <= 90))
          )
       {
          sprintf(s, "('%c' 0x%x)", c, (int) c);
       } else {
          sprintf(s, "('?' 0x%x)", (int) c);
       }
       *res+=s;
     }
     return (*res);
   }


};

#endif

