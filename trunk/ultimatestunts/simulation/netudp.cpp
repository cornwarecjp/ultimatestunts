/***************************************************************************
                          netudp.cpp - low level UDP network interface
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


#include "netudp.h"



CNetUDP::CNetUDP()
{
  sockfd = -1;
}

bool CNetUDP::setNetwork(const CString & localHost, int port) {

  struct sockaddr_in servaddr;


  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "Cannot open socket\n");
    return false;
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);     // fixme: to localhost
  servaddr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
   fprintf(stderr, "Cannot bind to port %d\n", port);
   return false;
  }

  fcntl(sockfd, F_SETFL, O_NONBLOCK);
  return true;
}

SNetDataGram * CNetUDP::recvData() const {
  int nread = -1;
  char unsigned msg[UDP_MAX_DATAGRAM];
  struct sockaddr_in from;
  socklen_t fromlen = sizeof(from);
  SNetDataGram *res = new SNetDataGram;


  nread = recvfrom(sockfd, &msg[0], UDP_MAX_DATAGRAM-1, 0, (struct sockaddr *) &from, &fromlen);

  if (nread > 0) {
   msg[nread] = 0;
   res->data = &msg[0];
   return (res);
  }

  return (NULL);

}

bool CNetUDP::sendData(const SNetDataGram & d) const {
  struct sockaddr_in destaddr;
  char *dat = d.data.raw_str();

  bzero(&destaddr, sizeof(destaddr));
  destaddr.sin_family = AF_INET;
  inet_pton(AF_INET, d.host.c_str(), &destaddr.sin_addr);
  destaddr.sin_port = htons(d.port);

  int res = sendto(sockfd, dat, sizeof(*dat),0, (struct sockaddr *) &destaddr, sizeof(destaddr));

  if (res == -1) return (false);

  return (true);

}


