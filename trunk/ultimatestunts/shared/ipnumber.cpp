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

#include <cstdio>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "ipnumber.h"



CIPNumber::CIPNumber()
{};

CIPNumber::CIPNumber(const CIPNumber & c)
{
	for (int i=0;i<4;i++)
		m_IP[i] = c[i];
}

CIPNumber::~CIPNumber()
{};


CIPNumber & CIPNumber::operator = (const CString & s)
{
	//converts string with hostname or IP address to struct
	struct hostent *h = gethostbyname(s.c_str());

	if(h==NULL)
	{
    	printf("unknown host '%s' \n", s.c_str());
		exit(1);
	}

	if(h->h_addrtype != AF_INET)
	{
    	printf("Error: only IPv4 is supported\n");
		exit(1);
	}

	for (int i=0;i<4;i++)
		m_IP[i] = h->h_addr_list[0][i];

	return (*this);
}

CIPNumber & CIPNumber::operator = (u_int32_t s_addr)
{
	Uint8 *ptr = (Uint8 *)(& s_addr);

	for (int i=0;i<4;i++)
		m_IP[i] = ptr[i];
	
	return (*this);
}

Uint8 CIPNumber::operator[] (const Uint8 ix) const
{
	return (m_IP[ix]);
}

bool CIPNumber::operator==(const CIPNumber & two) const
{
	for (Uint8 i=0;i<4;i++)
		if (m_IP[i] != two[i])
			return (false);

	return (true);
}

CString & CIPNumber::toString () const
{
	char *tmp = new char[3*4+3];
	sprintf(tmp, "%d.%d.%d.%d", m_IP[0], m_IP[1], m_IP[2], m_IP[3]);
	CString *res = new CString();
	*res = tmp;
	return (*res);
}

