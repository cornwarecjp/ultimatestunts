/***************************************************************************
                          filechunk.h  -  A file chunk message
                             -------------------
    begin                : ma nov 28 2005
    copyright            : (C) 2005 by CJP
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

#ifndef FILECHUNK_H
#define FILECHUNK_H

#include "message.h"
#include "cstring.h"

/**
  *@author CJP
  */

#define FILECHUNK_SIZE 1024
  
class CFileChunk : public CMessage
{
public:
	CString m_Filename;
	Uint32 m_ChunkID, m_FileSize;
	CBinBuffer m_Data;

	virtual bool setData(const CBinBuffer &b, unsigned int &pos)
	{
		m_Filename = b.getCString(pos);
		m_ChunkID = b.getUint32(pos);
		m_FileSize = b.getUint32(pos);

		//All other bytes:
		m_Data.clear();
		for(unsigned int i=pos; i < b.size(); i++)
			m_Data.push_back(b.getUint8(pos));

		return true;
	}

	virtual CBinBuffer &getData(CBinBuffer &b) const
	{
		b += m_Filename;
		b += m_ChunkID;
		b += m_FileSize;

		for(unsigned int i=0; i < m_Data.size(); i++)
			b += m_Data[i];

		return b;
	}

	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::fileChunk;}
};

#endif

