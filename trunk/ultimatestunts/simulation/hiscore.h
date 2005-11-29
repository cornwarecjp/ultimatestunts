/***************************************************************************
                          hiscore.h  -  Hiscore of a race, or a track
                             -------------------
    begin                : do nov 24 2005
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

#ifndef HISCORE_H
#define HISCORE_H

#include <vector>
namespace std {}
using namespace std;

#include "message.h"

/**
  *@author CJP
  */

struct SHiscoreEntry
{
	CString name;
	CString carname;
	float time;
	bool isNew; //is it from this race or not? (not saved in file of course)
};

class CHiscore : public CMessage, public vector<SHiscoreEntry> {
public: 
	CHiscore();
	virtual ~CHiscore();

	virtual CBinBuffer &getData(CBinBuffer &b) const;
	virtual bool setData(const CBinBuffer &b, unsigned int &pos);
	virtual CMessageBuffer::eMessageType getType() const {return CMessageBuffer::hiscore;}
};

#endif
