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

bool CNetUDP::setNetwork(const CIPNumber & localHost, int port) {

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

CNetUDP::SNetDatagram * CNetUDP::recvData() const {
  int nread = -1;
  char unsigned msg[UDP_MAX_DATAGRAM];
  struct sockaddr_in from;
  socklen_t fromlen = sizeof(from);
  SNetDatagram *res = new SNetDatagram;


  nread = recvfrom(sockfd, &msg[0], UDP_MAX_DATAGRAM-1, 0, (struct sockaddr *) &from, &fromlen);

  if (nread > 0) {
   char ip[14];
   msg[nread] = 0;
   if (inet_ntop(AF_INET, (struct sockaddr_in *) &from, ip, sizeof(ip)) != NULL) {
     res->host = &ip[0];
   }

   res->data = &msg[0];
   return (res);
  }

  return (NULL);

}

// check socket for read-, writablillity
bool CNetUDP::select(const bool sockread, const bool sockwrite, const int utimeout) const
{
  struct timeval selecttimeout;
  fd_set allset;
  fd_set *sread;
  fd_set *swrite;

  FD_ZERO(&allset);
  FD_SET(sockfd, &allset);


  selecttimeout.tv_sec = int (utimeout / 1000000);
  selecttimeout.tv_usec = int (utimeout % 1000000);

  if (sockread) sread=&allset; else sread=NULL;
  if (sockwrite) swrite=&allset; else swrite=NULL;

  if (::select(sockfd+1, sread, swrite, NULL, (struct timeval *) &selecttimeout) > 0) {
    return (true);
  }
  return (false);
}


bool CNetUDP::sendData(const SNetDatagram & d) const {
  struct sockaddr_in destaddr;
  char *dat = d.data.raw_str();

  bzero(&destaddr, sizeof(destaddr));
  destaddr.sin_family = AF_INET;
  inet_pton(AF_INET, d.host.toString().c_str(), &destaddr.sin_addr);
  destaddr.sin_port = htons(d.port);

  int res = sendto(sockfd, dat, sizeof(*dat),0, (struct sockaddr *) &destaddr, sizeof(destaddr));

  if (res == -1) return (false);

  return (true);

}


