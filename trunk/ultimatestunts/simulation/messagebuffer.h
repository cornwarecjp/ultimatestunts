/***************************************************************************
                          messagebuffer.h  -  Raw message containing type and data
                             -------------------
    begin                : Thu Dec 5 2002
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

#ifndef MESSAGEBUFFER_H
#define MESSAGEBUFFER_H

#include "binbuffer.h"



class CMessageBuffer : public CBinBuffer {
public:

typedef struct _s_msg_header {
		 Uint8 tid;		// type id
		 Uint8 ac;		// acknowledge
		 Uint16 counter; // package counter
	} s_msg_header;




enum eMessageType {   // more to add
   dummyMessage = 1,
   objectChoice = 2
};


private:
 s_msg_header m_HeaderCp;

 bool setHeader();
 bool rereadHeader();

public:
 CMessageBuffer(const CMessageBuffer::eMessageType & t);

 CMessageBuffer();

 bool setType(const eMessageType t);
 bool setAC(const Uint8 acflag);
 bool setCounter(const Uint16 counter);

 eMessageType getType();
 Uint8 getAC();
 Uint16 getCounter();


 bool setData(const CBinBuffer & b);
 CBinBuffer & getData() const;




 int getHeaderLength() const;

 bool setBuffer(const CBinBuffer & b);               // restore messagebuffer
 CBinBuffer & getBuffer();
	~CMessageBuffer();

};



#endif
