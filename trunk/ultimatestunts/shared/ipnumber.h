/***************************************************************************
                          ipnumber.h  -  ip number storage+converting class
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

#ifndef IPNUMBER_H
#define IPNUMBER_H

#include "SDL.h"
#include <vector>
namespace std {}
using namespace std;

#include "cstring.h"

class CIPNumber
{
  private:
   Uint8 m_IP[4];

  public:

   CIPNumber();
   CIPNumber(const CIPNumber & c);
   virtual ~CIPNumber();

   Uint8 operator[] (const Uint8 ix) const;
   CIPNumber & operator = (const char * c);
   CIPNumber & operator = (const CString & s);
   bool operator==(const CIPNumber & two) const;

   CString & toString () const;



};


#endif



