/***************************************************************************
                          netmessage.h - message network interface
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


// NOT YET TESTED!
//and commented because it isn't used
/*


#include "netmessage.h"


CNetMessage::SNetDatagram & CNetMessage::Msg2Dgram(const CMessageBuffer & m)
{
  CNetUDP::SNetDatagram *res = new CNetUDP::SNetDatagram;
  res->host = m.getIP();
  res->port = m.getPort();
  return (*res);
}

CMessageBuffer & CNetMessage::Dgram2Msg(const SNetDatagram & d)
{
  CMessageBuffer *res = new CMessageBuffer();
  res->setBuffer(d.data);
  res->setIP(d.host);
  res->setPort(d.port);
  return (*res);
}


bool CNetMessage::sendMessage(const CMessageBuffer & buffer)
{
  return (
   sendData(Msg2Dgram(buffer))
  );

}

CMessageBuffer * CNetMessage::recvMessage()
{
 SNetDatagram *dg = recvData();
 if (dg == NULL) return (NULL);
 CMessageBuffer *res = & Dgram2Msg(*dg);
 delete dg;
 return (res);
}

*/

