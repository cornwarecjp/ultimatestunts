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

#ifndef _NETMESSAGE_H
#define _NETMESSAGE_H

#include "netudp.h"
#include "SDL.h"
#include <vector>
namespace std {}
using namespace std;

#include "messagebuffer.h"



 class CNetMessage : public CNetUDP
 {
   private:
    struct ACQ {
      int long arrived;
      CMessageBuffer message;
    };

    static CNetMessage::SNetDatagram & Msg2Dgram(const CMessageBuffer & m);
    static CMessageBuffer & CNetMessage::Dgram2Msg(const CNetMessage::SNetDatagram & d);
    vector<struct ACQ> m_ACQueue;

   public:
    bool sendMessage(const CMessageBuffer & buffer);
    CMessageBuffer * recvMessage();                         // NULL = no more messages
 };


#endif

