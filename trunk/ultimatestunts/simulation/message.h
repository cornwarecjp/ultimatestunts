/***************************************************************************
                          message.h  -  description
                             -------------------
    begin                : Thu Dec 5 2002
    copyright            : (C) 2002 by CJP
    email                : cornware-cjp@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MESSAGE_H
#define MESSAGE_H


/**
  *@author CJP
  */

#include "messagebuffer.h"

class CMessage {

private:

  CMessageBuffer m_Data;
	
 	
public:
  CMessage();
  CMessage(const CMessageBuffer &);
  virtual ~CMessage();


  virtual CBinBuffer & getData() const = 0;         // returns class data as binbuffer
  virtual bool setData(const CBinBuffer &) = 0;   // rebuild class data from binbuffer

	CMessageBuffer & getBuffer();
 	bool setBuffer(const CMessageBuffer &);

	
      // header wrappers
  void setType(const CMessageBuffer::eMessageType & t);
 	void setCounter(const Uint16 & counter);
  void setAC(const bool & ac);
  CMessageBuffer::eMessageType getType() const { return ((CMessageBuffer::eMessageType) m_Data.getType()); }
  Uint16 getCounter() const { return (m_Data.getCounter()); }
  Uint8 getAC() const { return (m_Data.getAC()); }

	
};


/*
class CMessage {

public: 
	CMessage();
  CMessage(CMessageBuffer b);
	virtual ~CMessage();

  virtual CMessageBuffer getBuffer();
  virtual void setBuffer(CMessageBuffer b);
};

*/

#endif
