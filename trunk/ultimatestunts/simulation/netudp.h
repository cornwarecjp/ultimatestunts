/***************************************************************************
                          netudp.h - low level UDP network interface
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

// NOT TESTED!
//and commented because it isn't used
/*


#ifndef _cnetudp_h
#define _cnetudp_h

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstdio>
#include <unistd.h> // close() 
#include <string.h> // memset() 
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/select.h>

#include "binbuffer.h"
#include "ipnumber.h"


#define UDP_MAX_DATAGRAM 4096




class CNetUDP {

public:
typedef struct _SNetDatagram {
  CIPNumber host;
  Uint16 port;
  CBinBuffer data;
} SNetDatagram;


private:
  vector<SNetDatagram> m_inBuffer;

  int sockfd;


public:
  CNetUDP();
  bool select(const bool sockread=false, const bool sockwrite=false, const int utimeout=500000) const;    // own select() impl.
  bool sendData(const SNetDatagram &) const;
  SNetDatagram* recvData() const ;   // returns NULL if end of buffer

  bool setNetwork(const CIPNumber & localHost, const int port);

};

#endif
*/

