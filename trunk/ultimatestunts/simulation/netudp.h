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


#ifndef _cnetudp_h
#define _cnetudp_h

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close() */
#include <string.h> /* memset() */
#include <fcntl.h>
#include <sys/poll.h>

#include "binbuffer.h"


#define UDP_MAX_DATAGRAM 4096

typedef struct _SNetDataGram {
  CString host;
  Uint16 port;
  CBinBuffer data;
} SNetDataGram;

class CNetUDP {


private:
  vector<SNetDataGram> m_inBuffer;

  int sockfd;



public:
  CNetUDP();
  bool sendData(const SNetDataGram &) const;
  SNetDataGram* recvData() const ;   // NULL if end of buffer
  bool setNetwork(const CString & localHost, const int port);

};

#endif
