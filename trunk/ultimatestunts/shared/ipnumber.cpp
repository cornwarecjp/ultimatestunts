/***************************************************************************
                          ipnumber.h  -  ip number storage+converting class
                          TODO: this is IPv4 only :-(
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

#include "ipnumber.h"



CIPNumber::CIPNumber() {};
CIPNumber::CIPNumber(const CIPNumber & c) {
 for (int i=0;i<4;i++) { m_IP[i] = c[i]; }
}
CIPNumber::~CIPNumber() {};



CIPNumber & CIPNumber::operator = (const CString & s)
{
  return ((*this) = s.c_str());
}

Uint8 CIPNumber::operator[] (const Uint8 ix) const {
  return (m_IP[ix]);
}


CIPNumber & CIPNumber::operator = (const char * c)
{
    struct sockaddr_in addr;
    if (inet_pton(AF_INET, c, &addr) == 1) {
      const u_char *p = (const u_char *) &addr;         // this is a very dirty solution
      for (int i=0;i<4;i++) {
        m_IP[i] = p[i];
      }
    } else {
      fprintf(stderr, "Warning, bad IP addr: %s\n", c);
    }
    return (*this);
}


bool CIPNumber::operator==(const CIPNumber & two) const
{
 for (Uint8 i=0;i<4;i++) {
   if (m_IP[i] != two[i]) return (false);
 }
 return (true);
}

CString & CIPNumber::toString () const {
  char *tmp = new char[3*4+3];
  sprintf(tmp, "%d.%d.%d.%d", m_IP[0], m_IP[1], m_IP[2], m_IP[3]);
  CString *res = new CString();
  *res = tmp;
  return (*res);
}

